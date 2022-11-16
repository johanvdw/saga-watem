import subprocess
import os

# make sure saga vpo libs are added to path
os.environ["SAGA_TLB"]="build/network/topologize"


def test_topology(tmpdir):
    cmd = ["saga_cmd","topology","0", "-INPUTLINES", "tests/data/inputlines.shp", "-OUTPUTLINES", tmpdir/"outputlines.shp", "-SYSTEM", "tests/data/mask.rst"]
    result = subprocess.run(cmd,  capture_output=True)
    assert result.returncode==0
    print(result)

