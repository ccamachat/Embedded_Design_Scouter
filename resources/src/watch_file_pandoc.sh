fswatch ./Creating\ Custom\ Symbols\ and\ Footprints\ in\ KiCad.md
| xargs -n1 -I{} pandoc -H disable_float.tex -f markdown Creating\ Custom\ Symbols\ and\ Footprints\ in\ KiCad.md-t latex -o Creating\ Custom\ Symbols\ and\ Footprints\ in\ KiCad.pdf --lua-filter diagram-generator.lua
