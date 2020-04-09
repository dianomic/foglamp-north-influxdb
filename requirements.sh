#!/usr/bin/env bash

##--------------------------------------------------------------------
## Copyright (c) 2020 Dianomic Systems
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##--------------------------------------------------------------------

##
## Author: Mark Riddoch
##
git clone https://github.com/awegrzyn/influxdb-cxx.git
cd influxdb-cxx; mkdir build
cd build
echo Build and install Dynamic Client library - required for header files
cmake ..
make
sudo make install
echo Build and install static client library
rm -rf *
sed -i ../CMakeLists.txt -e 's/"Build shared versions of libraries" ON/"Build shared versions of libraries" OFF/'
cmake ..
make -i InfluxDB
sudo cp lib/libInfluxDB.a /usr/local/lib
