import tracemalloc

import numpy as np
import pickle
import heapq
from collections import deque
import time
import matplotlib.pyplot as plt
from tqdm import tqdm


# General Notes:
# - Update the provided file name (code_<RollNumber>.py) as per the instructions.
# - Do not change the function name, number of parameters or the sequence of parameters.
# - The expected output for each function is a path (list of node names)
# - Ensure that the returned path includes both the start node and the goal node, in the correct order.
# - If no valid path exists between the start and goal nodes, the function should return None.


# Algorithm: Iterative Deepening Search (IDS)

# Input:
#   - adj_matrix: Adjacency matrix representing the graph.
#   - start_node: The starting node in the graph.
#   - goal_node: The target node in the graph.

# Return:
#   - A list of node names representing the path from the start_node to the goal_node.
#   - If no path exists, the function should return None.

# Sample Test Cases:

#   Test Case 1:
#     - Start node: 1, Goal node: 2
#     - Return: [1, 7, 6, 2]

#   Test Case 2:
#     - Start node: 5, Goal node: 12
#     - Return: [5, 97, 98, 12]

#   Test Case 3:
#     - Start node: 12, Goal node: 49
#     - Return: None

#   Test Case 4:
#     - Start node: 4, Goal node: 12
#     - Return: [4, 6, 2, 9, 8, 5, 97, 98, 12]

def depth_limited_search(adj_matrix, current_node, goal_node, limit, visited):
    if current_node == goal_node:
        return [current_node]

    if limit <= 0:
        return None

    visited.add(current_node)
    for neighbor, connected in enumerate(adj_matrix[current_node]):
        if connected > 0 and neighbor not in visited:
            path = depth_limited_search(adj_matrix, neighbor, goal_node, limit - 1, visited)
            if path is not None:
                return [current_node] + path
    visited.remove(current_node)
    return None


def get_ids_path(adj_matrix, start_node, goal_node):
    max_depth = 28  # Upper bound for the depth limit based on the number of nodes
    limit = 0
    while limit <= max_depth:
        visited = set()
        path = depth_limited_search(adj_matrix, start_node, goal_node, limit, visited)
        if path is not None:
            return path
        limit += 1
    return None  # Return None if no path is found within the max_depth limit


# Algorithm: Bi-Directional Search

# Input:
#   - adj_matrix: Adjacency matrix representing the graph.
#   - start_node: The starting node in the graph.
#   - goal_node: The target node in the graph.

# Return:
#   - A list of node names representing the path from the start_node to the goal_node.
#   - If no path exists, the function should return None.

# Sample Test Cases:

#   Test Case 1:
#     - Start node: 1, Goal node: 2
#     - Return: [1, 7, 6, 2]

#   Test Case 2:
#     - Start node: 5, Goal node: 12
#     - Return: [5, 97, 98, 12]

#   Test Case 3:
#     - Start node: 12, Goal node: 49
#     - Return: None

#   Test Case 4:
#     - Start node: 4, Goal node: 12
#     - Return: [4, 6, 2, 9, 8, 5, 97, 98, 12]


def bfs_step(adj_matrix, queue, visited_from_this_side, visited_from_other_side, parent_map_this_side):
    current_node = queue.popleft()

    for neighbor, is_connected in enumerate(adj_matrix[current_node]):
        if is_connected and neighbor not in visited_from_this_side:
            visited_from_this_side.add(neighbor)
            parent_map_this_side[neighbor] = current_node
            queue.append(neighbor)

            if neighbor in visited_from_other_side:
                return neighbor

    return None


def construct_path(meeting_node, parent_from_start, parent_from_goal):
    path_from_start = []
    path_from_goal = []

    current = meeting_node
    while current != -1:
        path_from_start.append(current)
        current = parent_from_start.get(current, -1)

    current = parent_from_goal.get(meeting_node, -1)
    while current != -1:
        path_from_goal.append(current)
        current = parent_from_goal.get(current, -1)

    return path_from_start[::-1] + path_from_goal


def get_bidirectional_search_path(adj_matrix, start_node, goal_node):
    if start_node == goal_node:
        return [start_node]

    queue_start = deque([start_node])
    queue_goal = deque([goal_node])

    visited_start = {start_node}
    visited_goal = {goal_node}

    parent_start = {start_node: -1}
    parent_goal = {goal_node: -1}

    while queue_start and queue_goal:
        meeting_node = bfs_step(adj_matrix, queue_start, visited_start, visited_goal, parent_start)
        if meeting_node:
            return construct_path(meeting_node, parent_start, parent_goal)

        meeting_node = bfs_step(adj_matrix, queue_goal, visited_goal, visited_start, parent_goal)
        if meeting_node:
            return construct_path(meeting_node, parent_start, parent_goal)

    return None


# Algorithm: A* Search Algorithm

# Input:
#   - adj_matrix: Adjacency matrix representing the graph.
#   - node_attributes: Dictionary of node attributes containing x, y coordinates for heuristic calculations.
#   - start_node: The starting node in the graph.
#   - goal_node: The target node in the graph.

# Return:
#   - A list of node names representing the path from the start_node to the goal_node.
#   - If no path exists, the function should return None.

# Sample Test Cases:

#   Test Case 1:
#     - Start node: 1, Goal node: 2
#     - Return: [1, 7, 6, 2]

#   Test Case 2:
#     - Start node: 5, Goal node: 12
#     - Return: [5, 97, 28, 10, 12]

#   Test Case 3:
#     - Start node: 12, Goal node: 49
#     - Return: None

#   Test Case 4:
#     - Start node: 4, Goal node: 12
#     - Return: [4, 6, 27, 9, 8, 5, 97, 28, 10, 12]


def heuristic(node, goal_node, node_attributes):
    # Access the 'x' and 'y' coordinates from the dictionary properly
    x1 = float(node_attributes[node]['x'])
    y1 = float(node_attributes[node]['y'])
    x2 = float(node_attributes[goal_node]['x'])
    y2 = float(node_attributes[goal_node]['y'])

    return ((x1 - x2) ** 2 + (y1 - y2) ** 2) ** 0.5


def get_astar_search_path(adj_matrix, node_attributes, start_node, goal_node):
    open_list = []
    heapq.heappush(open_list, (0, start_node))
    came_from = {start_node: None}
    g_score = {start_node: 0}

    while open_list:
        _, current = heapq.heappop(open_list)

        if current == goal_node:
            path = []
            while current is not None:
                path.append(current)
                current = came_from[current]
            return path[::-1]

        for neighbor, connected in enumerate(adj_matrix[current]):
            if connected > 0:
                tentative_g_score = g_score[current] + connected
                if neighbor not in g_score or tentative_g_score < g_score[neighbor]:
                    g_score[neighbor] = tentative_g_score
                    f_score = tentative_g_score + heuristic(neighbor, goal_node, node_attributes)
                    heapq.heappush(open_list, (f_score, neighbor))
                    came_from[neighbor] = current

    return None


# Algorithm: Bi-Directional Heuristic Search

# Input:
#   - adj_matrix: Adjacency matrix representing the graph.
#   - node_attributes: Dictionary of node attributes containing x, y coordinates for heuristic calculations.
#   - start_node: The starting node in the graph.
#   - goal_node: The target node in the graph.

# Return:
#   - A list of node names representing the path from the start_node to the goal_node.
#   - If no path exists, the function should return None.

# Sample Test Cases:

#   Test Case 1:
#     - Start node: 1, Goal node: 2
#     - Return: [1, 7, 6, 2]

#   Test Case 2:
#     - Start node: 5, Goal node: 12
#     - Return: [5, 97, 98, 12]

#   Test Case 3:
#     - Start node: 12, Goal node: 49
#     - Return: None

#   Test Case 4:
#     - Start node: 4, Goal node: 12
#     - Return: [4, 34, 33, 11, 32, 31, 3, 5, 97, 28, 10, 12]

def other_construct_path(parents_forward, parents_backward, meeting_node):
    path_forward = []

    # Trace back from the meeting node to the start node in the forward search
    current_node = meeting_node
    while current_node is not None:
        path_forward.append(current_node)
        current_node = parents_forward.get(current_node, None)  # Get parent node or None

    path_forward = path_forward[::-1]  # Reverse the forward path

    # Trace forward from the meeting node to the goal node in the backward search
    path_backward = []
    current_node = meeting_node
    current_node = parents_backward.get(current_node, None)  # Move to the next node after meeting point

    while current_node is not None:
        path_backward.append(current_node)
        current_node = parents_backward.get(current_node, None)  # Get parent node or None

    # Combine forward and backward paths
    return path_forward + path_backward


def get_bidirectional_heuristic_search_path(adj_matrix, node_attributes, start_node, goal_node):
    forward_open = []
    backward_open = []

    heapq.heappush(forward_open, (0, start_node))
    heapq.heappush(backward_open, (0, goal_node))

    forward_came_from = {start_node: None}
    backward_came_from = {goal_node: None}

    forward_g_score = {start_node: 0}
    backward_g_score = {goal_node: 0}

    while forward_open and backward_open:
        # Forward search step
        if forward_open:
            _, current_forward = heapq.heappop(forward_open)
            if current_forward in backward_came_from:
                return other_construct_path(forward_came_from, backward_came_from, current_forward)

            for neighbor, connected in enumerate(adj_matrix[current_forward]):
                if connected > 0:
                    tentative_g_score = forward_g_score[current_forward] + connected
                    if neighbor not in forward_g_score or tentative_g_score < forward_g_score[neighbor]:
                        forward_g_score[neighbor] = tentative_g_score
                        f_score = tentative_g_score + heuristic(neighbor, goal_node, node_attributes)
                        heapq.heappush(forward_open, (f_score, neighbor))
                        forward_came_from[neighbor] = current_forward

        # Backward search step
        if backward_open:
            _, current_backward = heapq.heappop(backward_open)
            if current_backward in forward_came_from:
                return other_construct_path(forward_came_from, backward_came_from, current_backward)

            for neighbor, connected in enumerate(adj_matrix[current_backward]):
                if connected > 0:
                    tentative_g_score = backward_g_score[current_backward] + connected
                    if neighbor not in backward_g_score or tentative_g_score < backward_g_score[neighbor]:
                        backward_g_score[neighbor] = tentative_g_score
                        f_score = tentative_g_score + heuristic(neighbor, start_node, node_attributes)
                        heapq.heappush(backward_open, (f_score, neighbor))
                        backward_came_from[neighbor] = current_backward

    return None


# Bonus Problem

# Input:
# - adj_matrix: A 2D list or numpy array representing the adjacency matrix of the graph.

# Return:
# - A list of tuples where each tuple (u, v) represents an edge between nodes u and v.
#   These are the vulnerable roads whose removal would disconnect parts of the graph.

# Note:
# - The graph is undirected, so if an edge (u, v) is vulnerable, then (v, u) should not be repeated in the output list.
# - If the input graph has no vulnerable roads, return an empty list [].

def dfs_vulnerable(u, parent, visited, discovery, low, time, adj_matrix, bridges):
    visited[u] = True
    discovery[u] = low[u] = time[0]
    time[0] += 1

    for v, is_connected in enumerate(adj_matrix[u]):
        if is_connected:
            if not visited[v]:
                dfs_vulnerable(v, u, visited, discovery, low, time, adj_matrix, bridges)

                low[u] = min(low[u], low[v])

                if low[v] > discovery[u]:
                    bridges.append((u, v))

            elif v != parent:
                low[u] = min(low[u], discovery[v])


def bonus_problem(adj_matrix):
    n = len(adj_matrix)
    visited = [False] * n
    discovery = [float('inf')] * n
    low = [float('inf')] * n
    bridges = []
    time = [0]

    for i in range(n):
        if not visited[i]:
            dfs_vulnerable(i, -1, visited, discovery, low, time, adj_matrix, bridges)

    return bridges


def measure_memory_and_time(func, *args):
    tracemalloc.start()
    start_time = time.time()

    result = func(*args)

    end_time = time.time()
    time_taken = end_time - start_time
    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()

    return result, time_taken, peak


def compute_path_cost(path, adj_matrix):
    if path is None:
        return float('inf')  # No path, infinite cost
    total_cost = 0
    for i in range(len(path) - 1):
        total_cost += adj_matrix[path[i]][path[i + 1]]
    return total_cost


from joblib import Parallel, delayed
import pandas as pd


# Move process_pair outside of collect_data_for_all_pairs
def process_pair(start_node, end_node, adj_matrix, node_attributes):
    # Store the results for each algorithm for the current pair
    pair_results = {
        'BFS': None,
        'IDS': None,
        'A*': None,
        'Bidirectional Heuristic': None
    }

    # Measure BFS
    bfs_path, bfs_time, bfs_memory = measure_memory_and_time(get_bidirectional_search_path, adj_matrix,
                                                             start_node, end_node)
    bfs_cost = compute_path_cost(bfs_path, adj_matrix)
    pair_results['BFS'] = (bfs_time, bfs_memory / 1024, bfs_cost, bfs_path)

    if bfs_path is None:
        return pair_results

    # Measure IDS
    ids_path, ids_time, ids_memory = measure_memory_and_time(get_ids_path, adj_matrix, start_node, end_node)
    ids_cost = compute_path_cost(ids_path, adj_matrix)
    pair_results['IDS'] = (ids_time, ids_memory / 1024, ids_cost, ids_path)

    # Measure A*
    astar_path, astar_time, astar_memory = measure_memory_and_time(get_astar_search_path, adj_matrix,
                                                                   node_attributes, start_node, end_node)
    astar_cost = compute_path_cost(astar_path, adj_matrix)
    pair_results['A*'] = (astar_time, astar_memory / 1024, astar_cost, astar_path)

    # Measure Bidirectional Heuristic
    bidir_heuristic_path, bidir_heuristic_time, bidir_heuristic_memory = measure_memory_and_time(
        get_bidirectional_heuristic_search_path, adj_matrix, node_attributes, start_node, end_node)
    bidir_heuristic_cost = compute_path_cost(bidir_heuristic_path, adj_matrix)
    pair_results['Bidirectional Heuristic'] = (bidir_heuristic_time, bidir_heuristic_memory / 1024,
                                               bidir_heuristic_cost, bidir_heuristic_path)

    return pair_results


def collect_data_for_all_pairs(adj_matrix, node_attributes):
    n = len(adj_matrix)  # Number of nodes
    results = {
        'IDS': [],
        'BFS': [],
        'A*': [],
        'Bidirectional Heuristic': []
    }

    # Parallel processing for all node pairs
    all_results = Parallel(n_jobs=-1, backend="multiprocessing")(
        delayed(process_pair)(start_node, end_node, adj_matrix, node_attributes)
        for start_node in tqdm(range(n)) for end_node in range(n) if start_node != end_node
    )

    # Aggregating the results
    for res in all_results:
        results['BFS'].append(res['BFS'])
        results['IDS'].append(res['IDS'])
        results['A*'].append(res['A*'])
        results['Bidirectional Heuristic'].append(res['Bidirectional Heuristic'])

    # Saving results to CSV
    df = pd.DataFrame(results)
    df.to_csv('results.csv', index=False)

    return results


def generate_scatter_plots(results):
    # Unpack data
    algorithms = ['IDS', 'BFS', 'A*', 'Bidirectional Heuristic']

    for alg in algorithms:
        # Filter out None results
        filtered_results = [result for result in results[alg] if result is not None]

        # Ensure we have valid data to plot
        if len(filtered_results) > 0:
            times, memories, costs, paths = zip(*filtered_results)  # Extract time, memory, cost, and paths

            # Plot Time vs Memory
            plt.figure(figsize=(10, 6))
            plt.scatter(times, memories, alpha=0.7, label=alg)
            plt.title(f'Time vs Memory Usage - {alg}')
            plt.xlabel('Time (seconds)')
            plt.ylabel('Memory (KB)')
            plt.grid(True)
            plt.legend()
            plt.show()

            # Plot Time vs Cost
            plt.figure(figsize=(10, 6))
            plt.scatter(times, costs, alpha=0.7, label=alg)
            plt.title(f'Time vs Cost - {alg}')
            plt.xlabel('Time (seconds)')
            plt.ylabel('Cost (sum of edge weights)')
            plt.grid(True)
            plt.legend()
            plt.show()

            # Plot Memory vs Cost
            plt.figure(figsize=(10, 6))
            plt.scatter(memories, costs, alpha=0.7, label=alg)
            plt.title(f'Memory vs Cost - {alg}')
            plt.xlabel('Memory (KB)')
            plt.ylabel('Cost (sum of edge weights)')
            plt.grid(True)
            plt.legend()
            plt.show()
        else:
            print(f"No valid data for {alg}")


if __name__ == "__main__":
    adj_matrix = np.load('IIIT_Delhi.npy')
    with open('IIIT_Delhi.pkl', 'rb') as f:
        node_attributes = pickle.load(f)

    # Collect results for all pairs of nodes
    results = collect_data_for_all_pairs(adj_matrix, node_attributes)

    # Generate scatter plots to compare the algorithms
    generate_scatter_plots(results)

# if __name__ == "__main__":
#     adj_matrix = np.load('IIIT_Delhi.npy')
#     with open('IIIT_Delhi.pkl', 'rb') as f:
#         node_attributes = pickle.load(f)
#
#     for start_node in range(0, 125):
#         for end_node in range(0, 125):
#             print(start_node, end_node)
#             bidirectional_path = get_bidirectional_search_path(adj_matrix, start_node, end_node)
#             if bidirectional_path is not None:
#                 ids_path = get_ids_path(adj_matrix, start_node, end_node)
#                 astar_path = get_astar_search_path(adj_matrix, node_attributes, start_node, end_node)
#                 bidirectional_hurestic_path = get_bidirectional_heuristic_search_path(adj_matrix, node_attributes, start_node, end_node)
#
#             # print(f'Bidirectional Search Path: {get_bidirectional_search_path(adj_matrix, start_node, end_node)}')
#             # if get_bidirectional_search_path(adj_matrix, start_node, end_node) is not None:
#             #     print(f'Iterative Deepening Search Path: {get_ids_path(adj_matrix, start_node, end_node)}')
#             # print(f'A* Path: {get_astar_search_path(adj_matrix, node_attributes, start_node, end_node)}')
#             # print(
#             #     f'Bidirectional Heuristic Search Path: {get_bidirectional_heuristic_search_path(adj_matrix, node_attributes, start_node, end_node)}')
#             # print(f'Bonus Problem: {bonus_problem(adj_matrix)}')
