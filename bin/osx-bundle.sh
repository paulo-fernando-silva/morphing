#!/bin/sh

: ${1:?"bundle name must be given."}
bundle="$1"

# I would prefer not to use the version#, but I have more versions installed on my system
macdeployqt-4.8 $bundle

binary="`basename $bundle .app`"
contents="$bundle/Contents"
fw="Frameworks"
libdir="$contents/$fw"

# I'm using the Magick++ installed through macports included in the ImageMagick port. Replace directory as needed.
src_cdr="/opt/local/lib/ImageMagick-6.8.0/modules-Q16/coders"

if [ -d "$src_cdr" ]; then
    cp -R $src_cdr/{gif,mpeg,jpeg}.so "$libdir"
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

script_name="$binary.sh"
script="$contents/MacOS/$script_name"
plist="$contents/Info.plist"
sed "s/>$binary</>$script_name</" "$plist" > "$plist.tmp"
mv "$plist.tmp" "$plist"
echo "#!/bin/bash"                              >  "$script"
echo "wd=\"\${0%/*}\""                          >> "$script"
echo "export DYLD_LIBRARY_PATH=\"\$wd/../$fw\"" >> "$script"
echo "exe=\"\${wd}/$binary\""                   >> "$script"
echo "\"\$exe\""                                >> "$script"
chmod +x $script
echo "Done"

