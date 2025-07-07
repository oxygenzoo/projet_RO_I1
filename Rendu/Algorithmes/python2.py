import pandas as pd
import matplotlib.pyplot as plt

# Charger les données de max_values.txt
max_values = pd.read_csv("max_values.txt", delim_whitespace=True, header=None, names=["n", "time_FF", "time_PR", "time_MIN"])

# Vérification des données chargées
print(max_values.head())

# Tracer les courbes des temps maximaux pour chaque algorithme
plt.plot(max_values["n"], max_values["time_FF"], label="Ford-Fulkerson (FF)", marker="o")
plt.plot(max_values["n"], max_values["time_PR"], label="Push-Relabel (PR)", marker="s")
plt.plot(max_values["n"], max_values["time_MIN"], label="Min-Cost Flow (MIN)", marker="^")

# Configuration du graphique
plt.xscale("log")  # Échelle logarithmique pour n
plt.yscale("log")  # Facultatif : échelle logarithmique pour les temps si nécessaire
plt.xlabel("Taille du graphe (n)")
plt.ylabel("Temps maximal (s)")
plt.title("Comparaison des Temps Maximaux pour Chaque Algorithme")
plt.legend()
plt.grid(True)

# Afficher ou sauvegarder le graphique
plt.savefig("max_times_graph.png")  # Sauvegarde comme image
plt.show()

