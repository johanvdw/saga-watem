import os
import subprocess

import fiona
import geopandas as gpd
import numpy as np
import pandas as pd

# make sure saga vpo libs are added to path
os.environ["SAGA_TLB"]="build/network/topology"

def sagify(d: dict):
    """Convert a dict to a list of saga arguments

    eg convert {"INPUTLINES": "file.shp"} to
    ["-INPUTLINES", "file.shp"]
    """
    output_list = []
    for k in d:
        output_list += ["-"+k, d[k]]
    return output_list


def test_topology(tmpdir):
    outfile = tmpdir /"outputlines.shp"
    outputpoints = tmpdir /"outputpoints.shp"
    args = {"INPUTLINES": "tests/data/input_artificial.shp",
            "OUTPUTLINES": outfile,
            "OUTPUTPOINTS": outputpoints,
            "SYSTEM": "tests/data/mask.rst"
            }
    cmd = ["saga_cmd","topology","0"] + sagify(args)
    result = subprocess.run(cmd,  capture_output=True)
    assert result.returncode==0

    df=gpd.read_file(str(outfile))

    assert set(df.columns) == {"line_id", "startpt_id", "endpt_id", "length", "geometry"}
    assert len(df) == 6 # one more than original because it contains one multiline with two parts

    assert np.sum(df["endpt_id"]==2) ==2

    points = gpd.read_file(str(outputpoints))
    assert set(points) == {"point_id", "geometry"}
    assert len(points) == 7

def test_upstream_edges(tmpdir):
    adjecant_file = tmpdir /"adjecant.txt"
    upstream_file = tmpdir / "upstream.txt"
    args = {"INPUTLINES": "tests/data/topology.shp",
            "ADJECANT_EDGES": adjecant_file,
            "UPSTREAM_EDGES": upstream_file
            }

    cmd = ["saga_cmd","topology","1"] + sagify(args)
    result = subprocess.run(cmd,  capture_output=True)

    print(result.stderr)
    print(result.stdout)
    assert result.returncode==0

    adjecant = pd.read_csv(adjecant_file, sep="\t")
    upstream = pd.read_csv(upstream_file, sep="\t")

    assert set(adjecant.columns) == {"from", "to"}
    assert set(upstream.columns) == {"line_id", "upstream_line", "proportion"}

    assert len(adjecant) == 4
    assert len(upstream) == 9

    assert max(upstream["proportion"]) == 1

    assert np.sum(upstream["line_id"] == 5) == 4 # edge 5 has all other edges as upstream

def test_upstream_split(tmpdir):
    topology = tmpdir /"outputlines.shp"
    args = {"INPUTLINES": "tests/data/input_artificial_split.shp",
            "OUTPUTLINES": topology,
            "SYSTEM": "tests/data/mask.rst"
            }
    cmd = ["saga_cmd","topology","0"] + sagify(args)
    result = subprocess.run(cmd,  capture_output=True)
    assert result.returncode==0
    adjecant_file = tmpdir /"adjecant.txt"
    upstream_file = tmpdir / "upstream.txt"
    args = {"INPUTLINES": topology,
            "ADJECANT_EDGES": adjecant_file,
            "UPSTREAM_EDGES": upstream_file
            }

    cmd = ["saga_cmd","topology","1"] + sagify(args)

    result = subprocess.run(cmd,  capture_output=True)

    adjecant = pd.read_csv(adjecant_file, sep="\t")
    upstream = pd.read_csv(upstream_file, sep="\t")

    assert np.sum(upstream["proportion"] == 0.5) == 4

def test_upstream_circle(tmpdir):
    topology = tmpdir /"outputlines.shp"
    args = {"INPUTLINES": "tests/data/input_artificial_circle.shp",
            "OUTPUTLINES": topology,
            "SYSTEM": "tests/data/mask.rst"
            }
    cmd = ["saga_cmd","topology","0"] + sagify(args)
    result = subprocess.run(cmd,  capture_output=True)
    assert result.returncode==0
    adjecant_file = tmpdir /"adjecant.txt"
    upstream_file = tmpdir / "upstream.txt"
    args = {"INPUTLINES": topology,
            "ADJECANT_EDGES": adjecant_file,
            "UPSTREAM_EDGES": upstream_file
            }

    cmd = ["saga_cmd","topology","1"] + sagify(args)

    result = subprocess.run(cmd,  capture_output=True)

    adjecant = pd.read_csv(adjecant_file, sep="\t")
    upstream = pd.read_csv(upstream_file, sep="\t")
    assert np.sum(upstream["proportion"] < 1) == 0
