import pandas as pd
import matplotlib.pyplot as plt

def plot_aopc(csv_path="output/benchmark_results.csv", output_png="output/aopc_curve.png"):
    # 1. Load the benchmark data
    df = pd.read_csv(csv_path)

    # 2. Setup publication-style figure
    plt.figure(figsize=(7, 4.5), dpi=300)

    # Plot SHAP trajectory (the steep descent curve)
    plt.plot(
        df["step"],
        df["shap_score"],
        marker="o",
        color="#1f77b4",
        linewidth=2,
        label="SHAP Guided Perturbation"
    )

    # Plot Random Baseline trajectory
    plt.plot(
        df["step"],
        df["random_mean"],
        marker="s",
        linestyle="--",
        color="#7f7f7f",
        linewidth=2,
        label="Random Baseline (Monte Carlo)"
    )

    # If std dev is computed and non-zero, shade the error region
    if (df["random_std"] > 0).any():
        plt.fill_between(
            df["step"],
            df["random_mean"] - df["random_std"],
            df["random_mean"] + df["random_std"],
            color="#7f7f7f",
            alpha=0.2,
            label="Random ± 1 SD"
        )

    # Annotate key genes on the SHAP curve
    for _, row in df.iterrows():
        if row["step"] > 0:
            plt.annotate(
                f"Gene {int(row['gene_index'])}",
                (row["step"], row["shap_score"]),
                textcoords="offset points",
                xytext=(0, -15),
                ha="center",
                fontsize=8,
                color="#1f77b4"
            )

    plt.title("AOPC Fidelity Benchmark: SHAP vs. Random Baseline", fontsize=12, fontweight="bold")
    plt.xlabel("Number of Perturbed Features (k)", fontsize=10)
    plt.ylabel("Model Prediction Score", fontsize=10)
    plt.xticks(df["step"])
    plt.ylim(0.0, 1.05)
    plt.grid(True, linestyle=":", alpha=0.6)
    plt.legend(frameon=True, loc="upper right")
    plt.savefig(output_png, bbox_inches="tight")


    plt.savefig(output_png)
    print(f"[Plotter] Figure successfully saved to: {output_png}")
    plt.show()

if __name__ == "__main__":
    plot_aopc()
