from datetime import datetime
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns


def load_and_process_data(filename):
    # Read CSV with specific format
    df = pd.read_csv(filename, parse_dates=["Draw Date"])

    # Split the "Winning Numbers" column into separate numbers
    winning_numbers = df["Winning Numbers"].str.split(" ", expand=True).astype(int)

    # Basic statistical analysis
    stats = {
        "Number Analysis": {
            "Most frequent numbers": winning_numbers.values.flatten().tolist(),
            "Mega Ball frequency": df["Mega Ball"].value_counts().head(),
            "Multiplier frequency": df["Multiplier"].value_counts().head(),
        },
        "Summary Statistics": {
            "Total drawings": len(df),
            "Date range": f"{df['Draw Date'].min()} to {df['Draw Date'].max()}",
            "Average Mega Ball": df["Mega Ball"].mean(),
            "Most common Multiplier": df["Multiplier"].mode()[0],
        },
    }

    # Calculate frequency of each main number
    all_numbers = winning_numbers.values.flatten()
    number_freq = pd.Series(all_numbers).value_counts().sort_index()

    return stats, number_freq


def analyze_sequential_correlations(filename):
    # Read CSV
    df = pd.read_csv(filename)

    # Split winning numbers into array
    winning_numbers = (
        df["Winning Numbers"].str.split(" ").apply(lambda x: [int(n) for n in x])
    )

    # Create pairs of adjacent numbers
    pairs = []
    for numbers in winning_numbers:
        for i in range(len(numbers) - 1):
            pairs.append((numbers[i], numbers[i + 1]))

    # Convert to DataFrame for analysis
    pairs_df = pd.DataFrame(pairs, columns=["Number", "Next_Number"])

    # Calculate correlation coefficient
    correlation = pairs_df["Number"].corr(pairs_df["Next_Number"])

    # Create heatmap
    plt.figure(figsize=(10, 8))
    pivot_table = pd.crosstab(pairs_df["Number"], pairs_df["Next_Number"])
    sns.heatmap(pivot_table, cmap="YlOrRd")
    plt.title("Adjacent Number Correlation Heatmap")
    plt.savefig("number_correlations.png")
    plt.close()

    return {
        "correlation_coefficient": correlation,
        "common_pairs": pairs_df.value_counts().head(10).to_dict(),
    }


def plot_analysis(stats, number_freq):

    # Create a figure with subplots
    fig = plt.figure(figsize=(15, 10))

    # Plot number frequency distribution
    plt.subplot(2, 2, 1)
    number_freq.plot(kind="bar")
    plt.title("Main Number Frequency Distribution")
    plt.xlabel("Number")
    plt.ylabel("Frequency")

    # Plot Mega Ball frequency
    plt.subplot(2, 2, 2)
    stats["Number Analysis"]["Mega Ball frequency"].plot(kind="bar")
    plt.title("Mega Ball Frequency (Top 5)")
    plt.xlabel("Mega Ball Number")
    plt.ylabel("Frequency")

    # Plot Multiplier frequency
    plt.subplot(2, 2, 3)
    stats["Number Analysis"]["Multiplier frequency"].plot(kind="bar")
    plt.title("Multiplier Frequency (Top 5)")
    plt.xlabel("Multiplier")
    plt.ylabel("Frequency")

    # Add summary statistics as text
    plt.subplot(2, 2, 4)
    plt.axis("off")
    summary = "\n".join(
        [f"{key}: {value}" for key, value in stats["Summary Statistics"].items()]
    )
    plt.text(0.1, 0.5, f"Summary Statistics:\n\n{summary}", fontsize=10)

    plt.tight_layout()
    plt.show()


def generate_weighted_numbers(stats, number_freq, num_sets=5):
    # Normalize frequencies to create weights
    main_weights = number_freq / number_freq.sum()
    mega_ball_freq = stats["Number Analysis"]["Mega Ball frequency"]

    # Ensure mega ball weights cover full range 1-25
    mega_weights = pd.Series(0.0001, index=range(1, 26))  # Initialize with small values
    mega_weights.update(mega_ball_freq)  # Update with actual frequencies
    mega_weights = mega_weights / mega_weights.sum()  # Normalize

    generated_sets = []
    for _ in range(num_sets):
        # Generate 5 unique main numbers (1-70)
        main_numbers = []
        available_numbers = list(range(1, 71))

        for _ in range(5):
            # Create weights array for available numbers
            curr_weights = np.array(
                [main_weights.get(n, 0.0001) for n in available_numbers]
            )
            curr_weights = curr_weights / curr_weights.sum()  # Ensure sum to 1

            chosen = np.random.choice(available_numbers, p=curr_weights)
            main_numbers.append(chosen)
            available_numbers.remove(chosen)

        # Generate Mega Ball (1-25)
        mega_ball = np.random.choice(range(1, 26), p=mega_weights)
        generated_sets.append((sorted(main_numbers), mega_ball))

    return generated_sets


# Update main block
if __name__ == "__main__":
    try:
        stats, number_freq = load_and_process_data(
            "data/Lottery_Mega_Millions_Winning_Numbers__Beginning_2002.csv"
        )
        plot_analysis(stats, number_freq)
        generate_weighted_numbers(stats, number_freq)

        print("\nGenerated Number Sets Based on Historical Patterns:")
        number_sets = generate_weighted_numbers(stats, number_freq)
        for i, (main_nums, mega) in enumerate(number_sets, 1):
            print(f"Set {i}: Main Numbers: {main_nums}, Mega Ball: {mega}")

    except Exception as e:
        print(f"An error occurred: {str(e)}")
