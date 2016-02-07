#!/bin/sh

: ${1:?"bundle name must be given."}
bundle="$1"
deploy="${2:-macdeployqt}"

${deploy} "${bundle}"

binary="`basename $bundle .app`"
contents="$bundle/Contents"
fw="Frameworks"
libdir="$contents/$fw"

if [ ! -d "${libdir}" ]; then
    echo "${libdir} doesn't exist!"
    exit 1
fi

# I'm using the Magick++ installed through macports included in the ImageMagick port. Replace directory as needed.
VER="`Magick++-config --version | awk '{print $1}'`"
src_cdr="/opt/local/lib/ImageMagick-${VER}/modules-Q16/coders"

if [ -d "$src_cdr" ]; then
    cp -R ${src_cdr}/{gif,mpeg,jpeg}.so "$libdir"
    tag=' (compatibility '

    for so in $libdir/{gif,mpeg,jpeg}.so; do
        deps=`otool -L $so | grep $tag | sed "s/$tag.*//"`
    
        for dep in $deps; do
            dep_name="`basename $dep`"

            if [ -f "$libdir/$dep_name" ]; then
                install_name_tool -change "$dep" "@executable_path/../$fw/$dep_name" "$so"
            fi
        done
    done
else
    echo "$src not found. Edit this script and change the directory to match where you have the ImageMagick coders installed."
fi

# if [ ! -f "$libdir/libclparser.dylib" ]; then 
#     cp /System/Library/Frameworks/OpenCL.framework/Versions/A/Libraries/libclparser.dylib "$libdir"
# fi

script_name="$binary"
bin="$binary.app"
script="$contents/MacOS/$script_name"
mv $script "$contents/MacOS/$bin"
echo "#!/bin/bash"                              >  "$script"
echo "wd=\"\${0%/*}\""                          >> "$script"
echo "export DYLD_LIBRARY_PATH=\"\$wd/../$fw\"" >> "$script"
echo "cd \"\${wd}/../../../\""                  >> "$script"
echo "exe=\"\${wd}/$bin\""                      >> "$script"
echo "\"\$exe\""                                >> "$script"
chmod +x $script
echo "Done"

