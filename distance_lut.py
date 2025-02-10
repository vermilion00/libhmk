# https://www.desmos.com/calculator/nzl6twp6ui

from decimal import Decimal
from math import log2
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-a",
        type=Decimal,
        required=True,
        description="Constant obtained from fitting the curve",
    )
    parser.add_argument(
        "-i", type=int, default=1024, description="Number of entries in the LUT"
    )
    parser = parser.parse_args()

    a: Decimal = parser.a
    i: int = parser.i

    lut = []
    for x in range(i):
        numer = Decimal(255) * (Decimal(1) + a * Decimal(x)).log10()
        denom = (Decimal(1) + a * Decimal(i)).log10()
        lut.append(round(numer / denom))

    print(f"{{{", ".join(str(x) for x in lut)}}}")
