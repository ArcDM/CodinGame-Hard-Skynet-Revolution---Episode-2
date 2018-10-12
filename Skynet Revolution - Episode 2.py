"""
    This program takes an input of a set of nodes and their connections,
    a set of exit nodes, as well as a constant input of an agent's current node.
    The program outputs a connection to destroy to ultimately cut off
    the agent from all of the the exit nodes.
"""

class Node:
    def __init__( self, input_connection ):
        self.connections = [ input_connection ]
        self.distance = -1

node_count, link_count, exit_count = ( int( input_data ) for input_data in input().split() )
critical_nodes = []     # nodes connected to at least two gateways
exit_connections = []   # nodes connected to at least one gateway
node_network = {} 

for count in range( link_count ): # input_nodes defines a link between these nodes
    input_node1, input_node2 = ( int(input_data) for input_data in input().split() )
    
    if input_node1 in node_network: # if the node exists, add to its connections
        node_network[ input_node1 ].connections.append( input_node2 )
    else: # if the node down not exist, add the framework for that node
        node_network.update( { input_node1: Node( input_node2 ) } )
        
    if input_node2 in node_network: # if the node exists, add to its connections
        node_network[ input_node2 ].connections.append( input_node1 )
    else: # if the node down not exist, add the framework for that node
        node_network.update( { input_node2: Node( input_node1 ) } )
        
exit_indexes = [ int( input() ) for input_data in range( exit_count ) ]  # the indexes of gateway nodes

for index in exit_indexes:
    for link in node_network[index].connections:
        if link not in critical_nodes:
            if link in exit_connections: # link has more than one gateway connection
                critical_nodes.append( link )
            else: # not in either list
                exit_connections.append( link )

while exit_connections:
    agent_index = int( input() ) # The index of the node on which the Skynet agent is positioned this turn
    
    if agent_index in exit_connections: # if the agent is next to a gateway
        remove_node = agent_index
        
    elif critical_nodes: # if there are critical nodes, remove the 'closest' one
        for index in node_network.keys() - exit_indexes: # all nodes except gateways
            node_network[ index ].distance = None # set all nodes to None
    
        queue = [ agent_index ]
        node_network[ agent_index ].distance = 1
        remove_distance = node_count

        for index in queue: # set distances for all nodes
            if index in exit_connections:
                node_network[ index ].distance -= 1 # wont count as time to remove other connections
            
            for link in node_network[ index ].connections:
                if not node_network[ link ].distance: # if distance has not been set
                    queue.append( link )
                    node_network[ link ].distance = node_network[index].distance + 1
                    
                elif node_network[ link ].distance > node_network[index].distance + 1:
                    node_network[ link ].distance = node_network[index].distance + 1

        for index in critical_nodes: # search for the 'closest' critical node
            if remove_distance > node_network[ index ].distance:
                remove_distance = node_network[ index ].distance
                remove_node = index

    else: # chose any connection to cut
        remove_node = exit_connections[ 0 ]

    for link in node_network[ remove_node ].connections: # search for a gateway node
        if link in exit_indexes:
            node_network[ remove_node ].connections.remove( link )
            node_network[ link ].connections.remove( remove_node )
            
            print( remove_node, link )
            
            if remove_node in exit_connections:
                if remove_node in critical_nodes:
                    temporary_list = []
                
                    for index in exit_indexes:
                        temporary_list += node_network[ index ].connections
                    
                    if temporary_list.count( remove_node ) < 2: # if the node only has one gateway connection
                        critical_nodes.remove( remove_node )
                        
                else: # there was only one gateway connection
                    exit_connections.remove( remove_node )
            
            if not node_network[ remove_node ].connections: # if the node has no links
                del node_network[ remove_node ]       # remove the node
            
            if not node_network[ link ].connections: # if the node has no links
                del node_network[ link ]              # remove the node
                exit_indexes.remove( link )   # remove from gateway list

            break