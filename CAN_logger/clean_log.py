import csv

def search_csv(filename, field, query):
    matches = []
    with open(filename, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            if query.lower() in row[field].lower():
                matches.append(row)
    return matches

def print_results(results):
    if not results:
        print("No matches found.")
        return
    for row in results:
        print(", ".join(f"{k}: {v}" for k, v in row.items()))

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Search CSV log file.")
    parser.add_argument("filename", help="Path to CSV file")
    parser.add_argument("field", help="Field to search (e.g. timestamp, node_id, message_type)")
    parser.add_argument("query", help="Search term")

    args = parser.parse_args()

    results = search_csv(args.filename, args.field, args.query)
    print_results(results)