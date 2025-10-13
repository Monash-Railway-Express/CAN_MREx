import os
import pandas as pd
import dash
from dash import dcc, html, Input, Output
import plotly.express as px

# Path to the Logs folder (relative to the script location)
script_dir = os.path.dirname(os.path.abspath(__file__))
logs_dir = os.path.join(script_dir, "Logs")

# List CSV files inside Logs
csv_files = [f for f in os.listdir(logs_dir) if f.lower().endswith('.csv')][::-1]

external_stylesheets = [
    {
        "href": "https://fonts.googleapis.com/css2?family=Lato:wght@400;700&display=swap",
        "rel": "stylesheet",
    }
]

app = dash.Dash(__name__, external_stylesheets=external_stylesheets)
app.title = "CAN Bus Dashboard"
app.layout = html.Div(
    children=[
        # Header with image on the left
        html.Div(
            children=[
                html.Img(
                    src="/assets/MREx_logo.png",
                    className="header-image"
                ),
                html.Div(
                    children=[
                        html.H1("CAN Bus Dashboard", className="header-title"),
                        html.P(
                            "Explore CAN messages over time and analyze",
                            className="header-description",
                        ),
                    ],
                    className="header-text"
                ),
            ],
            className="header-container"
        ),

        # File selector menu
        html.Div(
            children=[
                html.Div("Select CSV File", className="menu-title"),
                dcc.Dropdown(
                    id="file-selector",
                    options=[{"label": f, "value": f} for f in csv_files],
                    value=csv_files[0] if csv_files else None,
                    clearable=False,
                    className="dropdown",
                ),
            ],
            className="file-menu",
        ),

        # ID selector
        html.Div(
            children=[
                html.Div("Select IDs", className="menu-title"),
                dcc.Dropdown(id="id-selector", multi=True, className="dropdown"),
            ],
            className="selector",
        ),

        # Time range slider
        html.Div(
            children=[
                html.Div("Select Time Range", className="menu-title"),
                dcc.RangeSlider(
                    id="time-range-slider",
                    step=1,
                    tooltip={"placement": "bottom", "always_visible": False},
                ),
            ],
            className="time-range-container",
        ),

        # Time graphs
        html.Div(
            children=[
                html.Div(
                    dcc.Graph(id="time-series-graph", config={"displayModeBar": False}),
                    className="card",
                ),
                html.Div(
                    dcc.Graph(id="occurrence-bar", config={"displayModeBar": False}),
                    className="card",
                ),
            ],
            className="graph-row",
        ),

        # Aggregation controls
        html.Div(
            children=[
                html.Div("Select Aggregation Interval", className="menu-title"),
                dcc.Dropdown(
                    id="agg-interval",
                    options=[
                        {"label": "1 Second", "value": "1s"},
                        {"label": "10 Seconds", "value": "10s"},
                        {"label": "1 Minute", "value": "1min"},
                    ],
                    value="1s",
                    clearable=False,
                    className="dropdown",
                ),
            ],
            className="selector",
        ),

        # Aggregation graphs
        html.Div(
            children=[
                html.Div(
                    dcc.Graph(id="agg-time-series", config={"displayModeBar": False}),
                    className="card",
                ),
                html.Div(
                    dcc.Graph(id="agg-bar", config={"displayModeBar": False}),
                    className="card",
                ),
            ],
            className="graph-row",
        ),
    ],
    className="page-container",
)

# --- Callbacks ---

@app.callback(
    Output("id-selector", "options"),
    Output("id-selector", "value"),
    Output("time-range-slider", "min"),
    Output("time-range-slider", "max"),
    Output("time-range-slider", "value"),
    Output("time-range-slider", "marks"),
    Input("file-selector", "value"),
)
def update_id_selector(filename):
    if not filename:
        return [], [], 0, 0, [0, 0], {}
    df = pd.read_csv(os.path.join(logs_dir, filename), engine="python")
    df["Timestamp"] = pd.to_datetime(df["Timestamp"])
    ids = sorted(df["ID"].unique())

    min_idx, max_idx = 0, len(df) - 1
    marks = {
        min_idx: str(df["Timestamp"].iloc[0]),
        max_idx: str(df["Timestamp"].iloc[-1]),
    }

    return (
        [{"label": i, "value": i} for i in ids],
        ids,
        min_idx,
        max_idx,
        [min_idx, max_idx],
        marks,
    )

@app.callback(
    Output("time-series-graph", "figure"),
    Output("occurrence-bar", "figure"),
    Input("file-selector", "value"),
    Input("id-selector", "value"),
    Input("time-range-slider", "value"),
)
def update_graph(filename, selected_ids, slider_range):
    if not filename or not selected_ids:
        return {}, {}

    df = pd.read_csv(os.path.join(logs_dir, filename), engine="python")
    df["Timestamp"] = pd.to_datetime(df["Timestamp"])
    df = df[df["ID"].isin(selected_ids)]

    if slider_range:
        start_idx, end_idx = slider_range
        df = df.iloc[start_idx:end_idx+1]

    counts = df["ID"].value_counts().reset_index()
    counts.columns = ["ID", "Count"]

    fig_time = px.scatter(df, x="Timestamp", y="ID", color="ID",
                          title="CAN Messages Over Time")
    fig_time.update_xaxes(rangeslider_visible=True)

    fig_bar = px.bar(counts, x="ID", y="Count", color="ID",
                     title="Occurrences per ID", text="Count")
    fig_bar.update_traces(textposition="outside")

    return fig_time, fig_bar
@app.callback(
    Output("agg-time-series", "figure"),
    Output("agg-bar", "figure"),
    Input("file-selector", "value"),
    Input("id-selector", "value"),
    Input("time-range-slider", "value"),   # ✅ use the existing slider
    Input("agg-interval", "value"),
)
def update_aggregated_graphs(filename, selected_ids, slider_range, agg_interval):
    if not filename or not selected_ids:
        return {}, {}

    df = pd.read_csv(os.path.join(logs_dir, filename), engine="python")
    df["Timestamp"] = pd.to_datetime(df["Timestamp"])
    df = df[df["ID"].isin(selected_ids)]

    if slider_range:
        start_idx, end_idx = slider_range
        df = df.iloc[start_idx:end_idx+1]

    # Set Timestamp as index, then group and resample
    df = df.set_index("Timestamp")
    agg_counts = (
        df.groupby("ID")
          .resample(agg_interval, include_groups=False)
          .size()
          .reset_index(name="Count")
    )

    # Time series of counts
    fig_time = px.line(
        agg_counts,
        x="Timestamp",
        y="Count",
        color="ID",
        title=f"Messages per {agg_interval}",
    )

    # Average counts per ID
    totals = agg_counts.groupby("ID")["Count"].mean().reset_index()
    fig_bar = px.bar(
        totals,
        x="ID",
        y="Count",
        color="ID",
        title=f"Average Messages per ID ({agg_interval})",
        text="Count",
    )
    fig_bar.update_traces(textposition="outside")

    return fig_time, fig_bar

# --- Run app ---
if __name__ == "__main__":
    app.run(debug=True)



# an error counter would be a good module
# inspect messages > a table for the given range
