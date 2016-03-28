set terminal png
set output "graph.png"
set key below
set autoscale xfix
plot [:][:] 'graph.data' using 2:1 title "performance time" smooth unique
