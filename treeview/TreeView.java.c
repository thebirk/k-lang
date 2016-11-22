import javax.swing.*;
import javax.swing.tree.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

/*

TODO:

- Select between searching from current node and downward or start from top
- 

*/

class Node
{
    Node parent;
    String value;
    ArrayList<Node> children = new ArrayList<Node>();
}

    public class TreeView {
        
        public static void convert_node(Node node, DefaultMutableTreeNode root)
        {
            DefaultMutableTreeNode me = new DefaultMutableTreeNode(node.value);
            root.add(me);
            for(int i = 0; i < node.children.size(); i++) {
                Node n = node.children.get(i);
                convert_node(n, me);
            }
        }
        
        public static TreePath find_path(DefaultMutableTreeNode root, String text)
        {
            Enumeration<DefaultMutableTreeNode> e = root.depthFirstEnumeration();
            while (e.hasMoreElements()) {
                DefaultMutableTreeNode node = e.nextElement();
                if (node.toString().startsWith(text)) {
                    return new TreePath(node.getPath());
                }
            }
            return null;
        }
        
        public static void do_search(String text, JTree tree, DefaultMutableTreeNode root)
        {
            TreePath path = find_path(root, text);
            tree.setSelectionPath(path);
            tree.scrollPathToVisible(path);
        }
        
        public static void create_tree(Node node)
        {
            try {
                UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
            } catch (Exception e) {
                e.printStackTrace();
                return;
            }
            
            DefaultMutableTreeNode root = new DefaultMutableTreeNode("root");
                        convert_node(node, root);
                        
                        JFrame f = new JFrame("TreeView");
                        f.setResizable(true);
                        f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            f.setLayout(new BorderLayout());
            
            JPanel search_panel = new JPanel();
            JTextField search_text = new JTextField(24);
            search_panel.add(search_text);
            
            JButton go = new JButton("Go");
            search_panel.add(go);
            search_panel.setLayout(new FlowLayout(FlowLayout.LEADING));
            f.add(search_panel, BorderLayout.PAGE_START);
            
                        JTree t = new JTree(root);
            t.setRootVisible(false);
            t.setShowsRootHandles(true);
            JScrollPane p = new JScrollPane(t);
                        f.add(p, BorderLayout.CENTER);
                        f.setSize(300, 300);
                        f.setLocationRelativeTo(null);
                        
            go.addActionListener(new ActionListener() {
                                 public void actionPerformed(ActionEvent e) {
                                     do_search(search_text.getText(), t, root);
                                 }
                                 });
                                 search_text.addActionListener(new ActionListener() {
                                                      public void actionPerformed(ActionEvent e) {
                                                      do_search(search_text.getText(), t, root);
                                                      }
                                                      });
            
                        f.setVisible(true);
            
            while(f.isShowing()) {}
        }
        
	public static void main(String[] args)
    {
        Node n = new Node();
        n.parent = null;
        n.value = "Hello";
        n.children = new ArrayList<Node>();
        
        Node c = new Node();
        c.value = "1";
        c.parent = n;
        c.children = new ArrayList<Node>();
        n.children.add(c);
        
        c = new Node();
        c.value = "2";
        c.parent = n;
        c.children = new ArrayList<Node>();
        n.children.add(c);
        
        create_tree(n);
	}
}

