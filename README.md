# network-latencies-bfs-c-language
Program used to calculate latencies of a network using BFS in C.



File "data.txt" contains all the necessary information about nodes, latencies, etc.
If you want to use your own network, follow the format of "data.txt", which is:

- numberOfNodes, numberOfEdges 								                                      (only one line)
- numberOfNode, queueTime									                                          (Many lines as nodes there are)
- originNodeLink, endNodeLink, linkVelocity, distanceOfLink, controlData, userData 	(many lines as links there are)
- finalSize 										                                                    (only one line)
- originNode, endNode									                                              (only one line. Nodes which we wanna know their latency) 

When you build and run the code, you need to introduce the name with the data you want to use, in this case "data.txt".
For different networks, you can modify "data.txt" or use another file.
