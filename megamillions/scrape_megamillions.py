import requests
from bs4 import BeautifulSoup
import pandas as pd
from datetime import datetime


def scrape_megamillions():
    # URL of the Mega Millions previous drawings
    url = "https://www.megamillions.com/Winning-Numbers/Previous-Drawings.aspx"

    # Send HTTP request
    response = requests.get(url)

    if response.status_code != 200:
        print(f"Failed to retrieve the webpage. Status code: {response.status_code}")
        return None

    # Parse HTML content
    soup = BeautifulSoup(response.content, "html.parser")

    # Find the table with winning numbers
    table = soup.find("table", {"class": "drawing-table"})

    if not table:
        print("Could not find the table with winning numbers")
        return None

    # Lists to store the data
    dates = []
    winning_numbers = []
    megaball_numbers = []
    multipliers = []

    # Parse each row in the table
    for row in table.find_all("tr")[1:]:  # Skip header row
        cols = row.find_all("td")
        if len(cols) >= 4:
            # Extract date
            date_str = cols[0].text.strip()
            date = datetime.strptime(date_str, "%m/%d/%Y")
            dates.append(date)

            # Extract winning numbers
            numbers = [num.text.strip() for num in cols[1].find_all("li")]
            winning_numbers.append(" ".join(numbers[:5]))  # First 5 numbers
            megaball_numbers.append(numbers[5])  # Mega Ball number

            # Extract multiplier
            multiplier = cols[3].text.strip()
            multipliers.append(multiplier)

    # Create DataFrame
    df = pd.DataFrame(
        {
            "Date": dates,
            "Winning Numbers": winning_numbers,
            "Mega Ball": megaball_numbers,
            "Multiplier": multipliers,
        }
    )

    return df


if __name__ == "__main__":
    # Scrape the data
    results = scrape_megamillions()

    if results is not None:
        # Save to CSV file
        results.to_csv("megamillions_results.csv", index=False)
        print("Results saved to megamillions_results.csv")
        print("\nFirst few entries:")
        print(results.head())
