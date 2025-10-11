import pandas as pd
import numpy as np

# Load your CSV (use engine="python" if you have ragged rows)
df = pd.read_csv("test2.csv", parse_dates=["Timestamp"], engine="python")

# Build a complete timeline from min to max
full_range = pd.date_range(start=df["Timestamp"].min(),
                           end=df["Timestamp"].max(),
                           freq="S")  # every second

# Reindex your dataframe to that full timeline
df = df.set_index("Timestamp").reindex(full_range).reset_index()
df = df.rename(columns={"index": "Timestamp"})

# Define the 4 IDs you want to randomly assign
id_choices = ["0x100", "0x200", "0x300", "0x400"]

# Fill missing IDs with random choices
mask = df["ID"].isna()
df.loc[mask, "ID"] = np.random.choice(id_choices, size=mask.sum())

# Save to a new CSV file
df.to_csv("test3.csv", index=False)
