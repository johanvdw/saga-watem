import subprocess
import os
import geopandas as gpd

# make sure saga vpo libs are added to path
os.environ["SAGA_TLB"]="build/network/topologize"


def test_topology(tmpdir):
    outfile = tmpdir /"outputlines.shp"
    cmd = ["saga_cmd","topology","0", "-INPUTLINES", "tests/data/inputlines.shp", "-OUTPUTLINES", outfile, "-SYSTEM", "tests/data/mask.rst"]
    result = subprocess.run(cmd,  capture_output=True)
    assert result.returncode==0

    df=gpd.read_file(str(outfile))
    assert len(df) == 18
    assert set(df.columns) == {"start_id", "end_id", "length", "geometry"}

