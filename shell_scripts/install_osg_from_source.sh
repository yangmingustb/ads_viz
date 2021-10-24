set -e

cd /tmp
rm -rf OpenSceneGraph
git clone --depth 1 --branch OpenSceneGraph-3.6.4 git@github.com:openscenegraph/OpenSceneGraph.git
cd OpenSceneGraph && mkdir build && cd build
cmake -DOSG_TEXT_USE_FONTCONFIG=OFF ..
make -j$(nproc)
sudo make install
sudo ldconfig