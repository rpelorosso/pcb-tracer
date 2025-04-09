#
# Estoy seguro de que en lugar de una lista de old_graph_id y new_graph_id puedo tener sólo una asociación old_graph_id -> new_graph_id sin guardar
# los ids de los links. Tendria que cambiarlo.
#

from PySide6.QtWidgets import QGraphicsView
from PySide6.QtCore import Qt, QPoint
from PySide6.QtGui import QPainter
from PySide6.QtGui import QMouseEvent
from zoomable_graphics_view import ZoomableGraphicsView
from link import Link, TrackGraph
from node import Node
from typing import Optional
from PySide6.QtGui import QUndoCommand, QUndoStack
from collections import namedtuple
from enums import LinkSide
from collections import deque

DeleteTrackMeta = namedtuple('DeleteTrackMeta', ['link_id'])

class DeleteTrack(QUndoCommand):
    def check_graph_split(self):
        # Perform BFS starting from fromNode, ignoring the link to be deleted
        visited_nodes, visited_links = self.bfs(self.fromNode, self.link.id)

        # If toNode is not in the visited set, the graph is split
        is_split = self.toNode not in visited_nodes
        graph = None
        graph_1 = [link for link in visited_links if link.id != self.link.id]

        if is_split:
            split_items = [item for item in self.editor.scene().items() 
                           if isinstance(item, Link) and item.graph_id == self.link.graph_id and item.id != self.link.id]
            
            #visited_link_ids = {link.id for link in visited_links}
            graph_2 = list(set(split_items) - visited_links)

            # if amount of items in graph_1 is 0 or graph_2 is 0, the graph is not split
            if len(graph_1) == 0:
                is_split = False
                graph = graph_2

            if len(graph_2) == 0:
                is_split = False
                graph = graph_1
        else:
            graph = visited_links

        if is_split:
            return True, graph_1, graph_2
        else:
            return False, graph, None
        
    def bfs(self, start_node, ignore_link):
        visited_nodes = set()
        visited_links = set()
        queue = deque([start_node])

        while queue:
            node = queue.popleft()
            if node not in visited_nodes:
                visited_nodes.add(node)
                for link in node.links:
                    if link.id != ignore_link:
                        visited_links.add(link)
                        next_node = link.to_node() if link.from_node() == node else link.from_node()
                        if next_node not in visited_nodes:
                            queue.append(next_node)

        return visited_nodes, visited_links

    def __init__(self, scene: ZoomableGraphicsView, meta: DeleteTrackMeta):
        super().__init__()

        self.editor = scene
        self.scene = scene.scene()
        self.layers = scene.layers
        self.meta = meta
        self.old_graph_ids = []
        self.new_graph_ids = []
        self.link = scene.find_item_by_id_and_class(self.meta.link_id, Link)

        self.fromNode = self.link.from_node()
        self.toNode = self.link.to_node()

        self.split_analysis = self.check_graph_split()

        # now check if one of it's nodes will have to be removed
        self.nodesToDelete = []
        self.nodesToUpdate = []

        self.deleteToNode = self.link.to_node().get_grade() == 1
        self.deleteFromNode  = self.link.from_node().get_grade() == 1            

        # calculate graph ids
        self.calculate_graph_ids()

    def undo(self):
        self.scene.addItem(self.link)
        if self.deleteToNode:
            self.scene.addItem(self.toNode)
        if self.deleteFromNode:
            self.scene.addItem(self.fromNode)
        
        self.link.setFromNode(self.fromNode)
        self.link.setToNode(self.toNode)

        self.fromNode.notify_link_changes()
        self.toNode.notify_link_changes()

        for item in self.old_graph_ids:
            link_to_change = self.editor.find_item_by_id_and_class(item["link_id"], Link)
            if link_to_change:
                link_to_change.graph_id = item["old_graph_id"]
                link_to_change.text_item.setText(f"{item["old_graph_id"]}")


    def redo(self):        
   
        # remove the link from the scene
        self.link.delete()

        if self.deleteToNode:
            self.toNode.will_be_deleted()
            self.scene.removeItem(self.toNode)
        else:
            self.toNode.notify_link_changes()

        if self.deleteFromNode:
            self.fromNode.will_be_deleted()
            self.scene.removeItem(self.fromNode)
        else:
            self.fromNode.notify_link_changes()

        for item in self.new_graph_ids:
            link_to_change = self.editor.find_item_by_id_and_class(item["link_id"], Link)
            if link_to_change:
                link_to_change.graph_id = item["new_graph_id"]
                link_to_change.text_item.setText(f"{item["new_graph_id"]}")

    def calculate_graph_ids(self):

        if self.split_analysis[0] == True: 
            # graph is split, we need to adjust the graph_ids
            # we'll rewrite the graph_ids of one of the newly created graphs 
            graph = self.split_analysis[1]
            # generate new graph id
            new_graph_id = TrackGraph.gen_track_graph_id()
            # update the links
            for link in graph:
                # add changed id to list of changed graph_id
                self.old_graph_ids.append({"link_id": link.id, "old_graph_id": link.graph_id})
                self.new_graph_ids.append({"link_id": link.id, "new_graph_id": new_graph_id})

    def to_dict(self):
        return {k: str(v) if isinstance(v, QPoint) else v for k, v in self.__dict__.items() if not k.startswith('_')}
