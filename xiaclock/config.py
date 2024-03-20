import json
import os

with open(os.path.join(os.getcwd(), "config.json"), "r", encoding="utf-8") as f:
    config = json.load(f)



def saveConfig():
    with open(os.path.join(os.getcwd(), "config.json"), "w", encoding="utf-8") as f:
        f.write(json.dump(config))

