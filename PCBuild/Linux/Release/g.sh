current="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/"
base="$(cd "${current}" && cd ../../../../ && pwd)/"
build="${base}build/Linux/GCC/Release/"

cmake -S "${base}" -B "${build}" -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release