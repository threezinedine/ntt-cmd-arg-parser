current="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/"
base="$(cd "${current}" && cd ../../../../ && pwd)/"
build="${base}build/Linux/GCC/Debug/"

cd "${base}"
make 
cd "${current}"
