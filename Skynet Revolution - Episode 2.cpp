#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <queue>

/**This program takes an input of a set of nodes and their connections, a
 * set of exit nodes, as well as a constant input of an agent's current node.
 * The program outputs a connection to destroy to ultimately cut off
 * the agent from all of the the exit nodes.
 */

struct Node
{ // used only for data storage
    std::unordered_set<int> links;
    int distance = -1;
        
    inline Node( const int &input_index)
    {
        links = { input_index };
    }
};

inline const void node_test_census( const std::unordered_map<int, Node*> &root_link ) // debug function
{ // This function counts out all the nodes in chain from the root_link and includes a list of links from those nodes
    int count = 0;

    if( root_link.empty() )
    { // if the root_link is empty
        std::cerr << "Empty node list" << std::endl;
        return;
    }
    
    for( const std::pair<int, Node*> &node_pointer: root_link )
    { // for every node in root_link
        std::cerr << "The node index: " << node_pointer.first << std::endl;
        
        if( node_pointer.second->links.empty() )
        { // if there are no links from this node
            std::cerr << "    Node has no links to other nodes" << std::endl;
        }
        else
        {
            for( const int &node_link: node_pointer.second->links )
            { // for every link in node_pointer
                std::cerr << "    Node index " << node_pointer.first << " links to node index " << node_link << std::endl;
            }
            
            count += node_pointer.second->links.size();
        }
    }
    
    std::cerr << "There are " << count/2 << " remaining links between nodes" << std::endl;
    // half because every link is counted from both nodes that share it
}

int main()
{
    std::unordered_map<int, Node*> node_network;
    std::unordered_set<int> exit_nodes, exit_connections, critical_nodes;
    int node_count, link_count, exit_count;
    
    std::cin >> node_count >> link_count >> exit_count; std::cin.ignore();
        
    for( int count = link_count, input_index1, input_index2; count; --count )
    {
        std::cin >> input_index1 >> input_index2; std::cin.ignore(); // input_indexes define a link between these nodes
            
        if( node_network.count( input_index1 ) )
        { // if the node already exists, add to set
            node_network[ input_index1 ]->links.emplace( input_index2 );
        }
        else
        { // if the node does not exist, make node
            node_network.emplace( input_index1, new Node( input_index2 ) );
        }
        
        if( node_network.count( input_index2 ) )
        { // if the node already exists, add to set
            node_network[ input_index2 ]->links.emplace( input_index1 );
        }
        else
        { // if the node does not exist, make node
            node_network.emplace( input_index2, new Node( input_index1 ) );
        }
    }
        
    for( int count = exit_count, exit_index; count; --count )
    {
        std::cin >> exit_index; std::cin.ignore(); // the index of a gateway node
        
        exit_nodes.emplace( exit_index );
        
        for( const int &link_index: node_network[exit_index]->links )
        { // for every link to this gateway node
            if( !critical_nodes.count( link_index ) )
            { // if link_index is not in critical_nodes
                if( exit_connections.count( link_index ) )
                { // link_index has more than one gateway connection
                    critical_nodes.emplace( link_index );
                }
                else
                { // not in either set
                    exit_connections.emplace( link_index );
                }
            }
        }
    }

    while( !exit_connections.empty() )
    {
        int agent_index, remove_node, remove_distance = node_count;
        std::cin >> agent_index; std::cin.ignore();

        if( std::find( exit_connections.begin(), exit_connections.end(), agent_index ) != exit_connections.end() )
        { // if the agent is next to a gateway
            remove_node = agent_index;
        }
        else
        {
            if( critical_nodes.empty() ) // if there are critical nodes, remove the 'closest' one
            { // chose any connection to cut
                remove_node = *exit_connections.begin();
            }
            else
            {
                for( const std::pair<int, Node*> &node_pointer: node_network )
                { // for all nodes
                    if( !exit_nodes.count( node_pointer.first ) )
                    { // if the node is not a gateway
                        node_pointer.second->distance = NULL; // set node distance to NULL
                    }
                }
    
                node_network[ agent_index ]->distance = 1;

                for( std::queue<int> pathfinding_queue ( { agent_index } ); !pathfinding_queue.empty(); pathfinding_queue.pop() )
                { // set distances for all nodes
                    if( exit_connections.count( pathfinding_queue.front() ) )
                    { // if the node is an exit connection
                        node_network[ pathfinding_queue.front() ]->distance -= 1; // wont count as time to remove other connections
                    }
            
                    for( const int &link_index: node_network[ pathfinding_queue.front() ]->links )
                    { // for every link to this node
                        if( !node_network[ link_index ]->distance )
                        { // if distance has not been set
                            node_network[ link_index ]->distance = node_network[ pathfinding_queue.front() ]->distance + 1;
                            pathfinding_queue.push( link_index ); // add to pathfinding_queue
                        }
                        else
                        {
                            if( node_network[ link_index ]->distance > node_network [pathfinding_queue.front() ]->distance + 1 )
                            { // if the distance is set and is further than next to this node
                                node_network[ link_index ]->distance = node_network[ pathfinding_queue.front() ]->distance + 1;
                            }
                        }
                    }
                }

                for( const int &node_index: critical_nodes )
                { // search for the 'closest' critical node
                    if( remove_distance > node_network[ node_index ]->distance )
                    {
                        remove_distance = node_network[ node_index ]->distance;
                        remove_node = node_index;
                    }
                }
            
            }
        }

        for( std::unordered_set<int>::iterator node_link = node_network[ remove_node ]->links.begin();
            node_link != node_network[ remove_node ]->links.end(); ++node_link )
        { // search for a gateway node, iteration used for deletion
            std::unordered_set<int>::iterator exit_address = exit_nodes.find( *node_link ); // iteration used for deletion
        
            if( exit_address != exit_nodes.end() )
            {
                std::cout << remove_node << " " << *node_link << std::endl;
                
                node_network[ *node_link ]->links.erase( remove_node ); // remove link from gateway
            
                if( node_network[ *node_link ]->links.empty() )
                { // if gateway node has no links
                    node_network.erase( *node_link ); // remove the node
                    exit_nodes.erase( exit_address ); // remove from gateway list
                }
                    
                node_network[ remove_node ]->links.erase( node_link ); // remove link to gateway
            
                if( node_network[ remove_node ]->links.empty() )
                { // if the node has no links
                    node_network.erase( remove_node ); // remove the node
                }
                
                std::unordered_set<int>::iterator node_address = critical_nodes.find( remove_node ); // find remove_node in critical_nodes
                
                if( node_address == critical_nodes.end() )
                { // node is not a critical node, remove it from exit_connections
                    exit_connections.erase( remove_node );
                }
                else
                { // node was a critical node
                    int exit_link_count = 0;
    
                    for( const int &exit_index: exit_nodes )
                    { // check every gateway node
                        exit_link_count += node_network[ exit_index ]->links.count( remove_node );
                    }

                    if( exit_link_count < 2 )
                    { // if the node only has one gateway connection left
                        critical_nodes.erase( node_address );
                    }
                }

                break;
            }
        }
    }
}