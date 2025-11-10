import java.io.*;
import java.util.*;

public class oving7 {
  static class Node {
    final int id;
    final double lat;
    final double lon;
    int poiType;
    String poiName;

    Node(int id, double lat, double lon) {
      this.id = id;
      this.lat = lat;
      this.lon = lon;
      this.poiType = 0;
      this.poiName = "";
    }

    @Override
    public String toString() {
      String poi = poiType > 0 ? "\t" + poiType + "\t" + poiName : "";
      return id + "\t" + lat + "\t" + lon + poi;
    }
  }

  static class Edge {
    final int from;
    final int to;
    final double travelTime;
    final double length;
    final double speedLimit;

    Edge(int from, int to, double travelTime, double length, double speedLimit) {
      this.from = from;
      this.to = to;
      this.travelTime = travelTime;
      this.length = length;
      this.speedLimit = speedLimit;
    }

    @Override
    public String toString() {
      return from + "\t" + to + "\t" + travelTime + "\t" + length + "\t" + speedLimit;
    }
  }

  static class Graph {
    List<Node> nodes;
    List<List<Edge>> adjacency;
    Map<Integer, Node> nodeMap;
    
    Graph(List<Node> nodes, List<Edge> edges) {
      this.nodes = nodes;
      this.nodeMap = new HashMap<>();
      
      for (Node n : nodes) {
        nodeMap.put(n.id, n);
      }
      
      int maxId = 0;
      for (Node n : nodes) {
        if (n.id > maxId) maxId = n.id;
      }
      
      this.adjacency = new ArrayList<>();
      for (int i = 0; i <= maxId; i++) {
        adjacency.add(new ArrayList<>());
      }
      
      for (Edge e : edges) {
        if (e.from < adjacency.size()) {
          adjacency.get(e.from).add(e);
        }
      }
    }
  }

  static class DijkstraNode implements Comparable<DijkstraNode> {
    int nodeId;
    double distance;
    
    DijkstraNode(int nodeId, double distance) {
      this.nodeId = nodeId;
      this.distance = distance;
    }
    
    @Override
    public int compareTo(DijkstraNode other) {
      return Double.compare(this.distance, other.distance);
    }
  }

  static class DijkstraResult {
    double[] distances;
    int[] previous;
    int nodesVisited;
    
    DijkstraResult(int size) {
      this.distances = new double[size];
      this.previous = new int[size];
      Arrays.fill(distances, Double.POSITIVE_INFINITY);
      Arrays.fill(previous, -1);
      this.nodesVisited = 0;
    }
  }

  static class Landmarks {
    int[] landmarkIds;
    double[][] distancesTo;
    double[][] distancesFrom;
    
    Landmarks(int numLandmarks, int graphSize) {
      this.landmarkIds = new int[numLandmarks];
      this.distancesTo = new double[numLandmarks][graphSize];
      this.distancesFrom = new double[numLandmarks][graphSize];
      
      for (int i = 0; i < numLandmarks; i++) {
        Arrays.fill(distancesTo[i], Double.POSITIVE_INFINITY);
        Arrays.fill(distancesFrom[i], Double.POSITIVE_INFINITY);
      }
    }
  }

  public static List<Node> loadNodes(String filename) throws IOException {
    List<Node> nodes = new ArrayList<>();
    BufferedReader br = new BufferedReader(new FileReader(filename));
    String line;
    while ((line = br.readLine()) != null) {
      if (line.trim().isEmpty()) continue;
      String[] parts = line.split("\\s+");
      if (parts.length < 3) continue;
      nodes.add(new Node(Integer.parseInt(parts[0]), 
                        Double.parseDouble(parts[1]), 
                        Double.parseDouble(parts[2])));
    }
    br.close();
    return nodes;
  }

  public static List<Edge> loadEdges(String filename) throws IOException {
    List<Edge> edges = new ArrayList<>();
    BufferedReader br = new BufferedReader(new FileReader(filename));
    String line;
    while ((line = br.readLine()) != null) {
      if (line.trim().isEmpty()) continue;
      String[] parts = line.split("\\s+");
      if (parts.length < 5) continue;
      edges.add(new Edge(Integer.parseInt(parts[0]), 
                        Integer.parseInt(parts[1]),
                        Double.parseDouble(parts[2]), 
                        Double.parseDouble(parts[3]), 
                        Double.parseDouble(parts[4])));
    }
    br.close();
    return edges;
  }

  public static void loadPOIs(List<Node> nodes, String filename) throws IOException {
    Map<Integer, Node> nodeMap = new HashMap<>();
    for (Node n : nodes) nodeMap.put(n.id, n);
    
    BufferedReader br = new BufferedReader(new FileReader(filename));
    br.readLine();
    String line;
    while ((line = br.readLine()) != null) {
      String[] parts = line.split("\t");
      if (parts.length >= 3) {
        Node node = nodeMap.get(Integer.parseInt(parts[0]));
        if (node != null) {
          node.poiType = Integer.parseInt(parts[1]);
          node.poiName = parts[2].replace("\"", "");
        }
      }
    }
    br.close();
  }

  public static DijkstraResult dijkstra(Graph graph, int start, int target) {
    DijkstraResult result = new DijkstraResult(graph.adjacency.size());
    PriorityQueue<DijkstraNode> pq = new PriorityQueue<>();
    
    result.distances[start] = 0;
    pq.offer(new DijkstraNode(start, 0));
    
    while (!pq.isEmpty()) {
      DijkstraNode current = pq.poll();
      int u = current.nodeId;
      
      if (current.distance > result.distances[u]) {
        continue;
      }
      
      result.nodesVisited++;
      
      if (target != -1 && u == target) {
        break;
      }
      
      for (Edge edge : graph.adjacency.get(u)) {
        int v = edge.to;
        if (v >= result.distances.length) {
          continue;
        }
        
        double newDist = result.distances[u] + edge.travelTime;
        
        if (newDist < result.distances[v]) {
          result.distances[v] = newDist;
          result.previous[v] = u;
          pq.offer(new DijkstraNode(v, newDist));
        }
      }
    }
    
    return result;
  }

  public static double altHeuristic(Landmarks landmarks, int node, int target) {
    double maxLower = 0;
    
    for (int i = 0; i < landmarks.landmarkIds.length; i++) {
      double toLandmark = landmarks.distancesTo[i][node];
      double targetToLandmark = landmarks.distancesTo[i][target];
      double fromLandmark = landmarks.distancesFrom[i][node];
      double targetFromLandmark = landmarks.distancesFrom[i][target];
      
      double lower1 = Math.abs(toLandmark - targetToLandmark);
      double lower2 = Math.abs(fromLandmark - targetFromLandmark);
      
      maxLower = Math.max(maxLower, Math.max(lower1, lower2));
    }
    
    return maxLower;
  }

  public static DijkstraResult altSearch(Graph graph, Landmarks landmarks, int start, int target) {
    DijkstraResult result = new DijkstraResult(graph.adjacency.size());
    PriorityQueue<DijkstraNode> pq = new PriorityQueue<>();
    
    result.distances[start] = 0;
    pq.offer(new DijkstraNode(start, 0));
    
    while (!pq.isEmpty()) {
      DijkstraNode current = pq.poll();
      int u = current.nodeId;
      
      if (current.distance > result.distances[u] + altHeuristic(landmarks, u, target)) {
        continue;
      }
      
      result.nodesVisited++;
      
      if (u == target) {
        break;
      }
      
      for (Edge edge : graph.adjacency.get(u)) {
        int v = edge.to;
        if (v >= result.distances.length) {
          continue;
        }
        
        double newDist = result.distances[u] + edge.travelTime;
        
        if (newDist < result.distances[v]) {
          result.distances[v] = newDist;
          result.previous[v] = u;
          double priority = newDist + altHeuristic(landmarks, v, target);
          pq.offer(new DijkstraNode(v, priority));
        }
      }
    }
    
    return result;
  }

  public static List<Integer> getPath(DijkstraResult result, int target) {
    List<Integer> path = new ArrayList<>();
    if (result.distances[target] == Double.POSITIVE_INFINITY) {
      return path;
    }
    
    for (int at = target; at != -1; at = result.previous[at]) {
      path.add(at);
    }
    
    Collections.reverse(path);
    return path;
  }

  public static String formatTime(double seconds) {
    int totalSeconds = (int) seconds;
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int secs = totalSeconds % 60;
    return String.format("%02d:%02d:%02d", hours, minutes, secs);
  }

  static class POIResult {
    int nodeId;
    double distance;
    Node node;
    List<Integer> path;
    
    POIResult(int nodeId, double distance, Node node, List<Integer> path) {
      this.nodeId = nodeId;
      this.distance = distance;
      this.node = node;
      this.path = path;
    }
  }

  public static List<POIResult> findNearestPOIs(Graph graph, int start, int poiType, int count) {
    List<POIResult> results = new ArrayList<>();
    double[] distances = new double[graph.adjacency.size()];
    int[] previous = new int[graph.adjacency.size()];
    Arrays.fill(distances, Double.POSITIVE_INFINITY);
    Arrays.fill(previous, -1);
    
    PriorityQueue<DijkstraNode> pq = new PriorityQueue<>();
    distances[start] = 0;
    pq.offer(new DijkstraNode(start, 0));
    
    while (!pq.isEmpty() && results.size() < count) {
      DijkstraNode current = pq.poll();
      int u = current.nodeId;
      
      if (current.distance > distances[u]) {
        continue;
      }
      
      Node node = graph.nodeMap.get(u);
      if (node != null && u != start) {
        int nodePOIType = node.poiType;
        if ((nodePOIType & poiType) != 0) {
          List<Integer> path = new ArrayList<>();
          for (int at = u; at != -1; at = previous[at]) {
            path.add(at);
          }
          Collections.reverse(path);
          results.add(new POIResult(u, distances[u], node, path));
          
          if (results.size() >= count) {
            break;
          }
        }
      }
      
      for (Edge edge : graph.adjacency.get(u)) {
        int v = edge.to;
        if (v >= distances.length) {
          continue;
        }
        
        double newDist = distances[u] + edge.travelTime;
        
        if (newDist < distances[v]) {
          distances[v] = newDist;
          previous[v] = u;
          pq.offer(new DijkstraNode(v, newDist));
        }
      }
    }
    
    return results;
  }

  public static Landmarks preprocessLandmarks(Graph graph, int numLandmarks) {
    Landmarks landmarks = new Landmarks(numLandmarks, graph.adjacency.size());
    
    selectLandmarks(graph, landmarks);
    
    System.out.println("Selected " + numLandmarks + " landmarks");
    
    for (int i = 0; i < numLandmarks; i++) {
      int landmarkId = landmarks.landmarkIds[i];
      System.out.println("Preprocessing landmark " + (i+1) + "/" + numLandmarks + " (node " + landmarkId + ")");
      
      DijkstraResult toResult = dijkstra(graph, landmarkId, -1);
      for (int j = 0; j < graph.adjacency.size(); j++) {
        landmarks.distancesFrom[i][j] = toResult.distances[j];
      }
      
      DijkstraResult fromResult = dijkstraReverse(graph, landmarkId);
      for (int j = 0; j < graph.adjacency.size(); j++) {
        landmarks.distancesTo[i][j] = fromResult.distances[j];
      }
    }
    
    return landmarks;
  }

  public static void selectLandmarks(Graph graph, Landmarks landmarks) {
    Random rand = new Random(42);
    Set<Integer> selected = new HashSet<>();
    
    for (int i = 0; i < landmarks.landmarkIds.length; i++) {
      int nodeId;
      do {
        nodeId = rand.nextInt(graph.nodes.size());
      } while (selected.contains(graph.nodes.get(nodeId).id));
      
      landmarks.landmarkIds[i] = graph.nodes.get(nodeId).id;
      selected.add(graph.nodes.get(nodeId).id);
    }
  }

  public static DijkstraResult dijkstraReverse(Graph graph, int start) {
    DijkstraResult result = new DijkstraResult(graph.adjacency.size());
    PriorityQueue<DijkstraNode> pq = new PriorityQueue<>();
    
    result.distances[start] = 0;
    pq.offer(new DijkstraNode(start, 0));
    
    while (!pq.isEmpty()) {
      DijkstraNode current = pq.poll();
      int u = current.nodeId;
      
      if (current.distance > result.distances[u]) {
        continue;
      }
      
      result.nodesVisited++;
      
      for (int v = 0; v < graph.adjacency.size(); v++) {
        for (Edge edge : graph.adjacency.get(v)) {
          if (edge.to == u) {
            double newDist = result.distances[u] + edge.travelTime;
            
            if (newDist < result.distances[v]) {
              result.distances[v] = newDist;
              result.previous[v] = u;
              pq.offer(new DijkstraNode(v, newDist));
            }
          }
        }
      }
    }
    
    return result;
  }

  public static void main(String[] args) throws Exception {
    if (args.length < 2) {
      System.out.println("Usage: java oving7 <nodesFile> <edgesFile> [poiFile] [startNode] [targetNode|POI] [numLandmarks]");
      return;
    }

    List<Node> nodes = loadNodes(args[0]);
    System.out.println("Loaded " + nodes.size() + " nodes");

    List<Edge> edges = loadEdges(args[1]);
    System.out.println("Loaded " + edges.size() + " edges");
    
    if (args.length > 2) {
      loadPOIs(nodes, args[2]);
      long poiCount = nodes.stream().filter(n -> n.poiType > 0).count();
      System.out.println("Loaded " + poiCount + " POIs");
    }
    
    Graph graph = new Graph(nodes, edges);
    System.out.println("Graph created");
    
    if (args.length >= 5) {
      int start = Integer.parseInt(args[3]);
      String targetArg = args[4];
      
      if (targetArg.startsWith("POI:")) {
        int poiType = Integer.parseInt(targetArg.substring(4));
        System.out.println("\n=== Finding 5 nearest POIs of type " + poiType + " from node " + start + " ===");
        
        long poiStart = System.nanoTime();
        List<POIResult> pois = findNearestPOIs(graph, start, poiType, 5);
        long poiEnd = System.nanoTime();
        
        double poiTime = (poiEnd - poiStart) / 1_000_000.0;
        System.out.println("Search time: " + String.format("%.2f", poiTime) + " ms");
        System.out.println("Found " + pois.size() + " POIs:\n");
        
        for (int i = 0; i < pois.size(); i++) {
          POIResult poi = pois.get(i);
          System.out.println((i+1) + ". " + poi.node.poiName);
          System.out.println("   Node: " + poi.nodeId);
          System.out.println("   Travel time: " + formatTime(poi.distance));
          System.out.println("   Distance: " + String.format("%.2f", poi.distance) + " seconds");
          System.out.println("   Path length: " + poi.path.size() + " nodes");
          if (poi.path.size() <= 20) {
            System.out.println("   Route: " + poi.path);
          }
          System.out.println();
        }
        return;
      }
      
      int target = Integer.parseInt(targetArg);
      int numLandmarks = args.length > 5 ? Integer.parseInt(args[5]) : 8;
      
      System.out.println("\n=== Dijkstra ===");
      System.out.println("Running Dijkstra from " + start + " to " + target);
      long dijkstraStart = System.nanoTime();
      DijkstraResult dijkstraResult = dijkstra(graph, start, target);
      long dijkstraEnd = System.nanoTime();
      
      double dijkstraTime = (dijkstraEnd - dijkstraStart) / 1_000_000.0;
      List<Integer> dijkstraPath = getPath(dijkstraResult, target);
      
      System.out.println("Algorithm time: " + String.format("%.2f", dijkstraTime) + " ms");
      System.out.println("Nodes visited: " + dijkstraResult.nodesVisited);
      System.out.println("Travel time: " + formatTime(dijkstraResult.distances[target]) + " (timer:minutter:sekunder)");
      System.out.println("Path length: " + dijkstraPath.size() + " nodes");
      if (dijkstraPath.size() <= 50) {
        System.out.println("Route: " + dijkstraPath);
      }
      
      System.out.println("\n=== ALT with " + numLandmarks + " landmarks ===");
      System.out.println("Preprocessing landmarks...");
      long prepStart = System.nanoTime();
      Landmarks landmarks = preprocessLandmarks(graph, numLandmarks);
      long prepEnd = System.nanoTime();
      double prepTime = (prepEnd - prepStart) / 1000.0;
      System.out.println("Preprocessing time: " + String.format("%.2f", prepTime) + " ms");
      
      System.out.println("\nRunning ALT from " + start + " to " + target);
      long altStart = System.nanoTime();
      DijkstraResult altResult = altSearch(graph, landmarks, start, target);
      long altEnd = System.nanoTime();
      
      double altTime = (altEnd - altStart) / 1_000_000.0;
      List<Integer> altPath = getPath(altResult, target);
      
      System.out.println("Algorithm time: " + String.format("%.2f", altTime) + " ms");
      System.out.println("Nodes visited: " + altResult.nodesVisited);
      System.out.println("Travel time: " + formatTime(altResult.distances[target]) + " (timer:minutter:sekunder)");
      System.out.println("Path length: " + altPath.size() + " nodes");
      if (altPath.size() <= 50) {
        System.out.println("Route: " + altPath);
      }
      
      System.out.println("\n=== Comparison ===");
      System.out.println("Speedup: " + String.format("%.2fx", dijkstraTime / altTime));
      System.out.println("Node reduction: " + String.format("%.1f%%", 100.0 * (dijkstraResult.nodesVisited - altResult.nodesVisited) / dijkstraResult.nodesVisited));
    }
  }
}