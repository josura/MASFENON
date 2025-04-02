import random
from pyvis import network as net
from IPython.display import display, HTML
 
def generate_edge():
  s = random.randint(1,10)
  d = random.randint(1,10)
  return (s,d)
 
def generate_size_node():
  v = random.randint(5,20)
  return v
 
def generate_color():
  return '#%06x' % random.randint(0, 0xFFFFFF)
 

# load the graph nodes and edges from
nodesFile = "/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/nodeValuesWithLR/AT1-metabolites.tsv"
edgesFile = "/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/graphsWithLR/AT1-metabolites.tsv"
g_complete =net.Network(height='600px',width='50%',
              bgcolor='white',font_color="red",notebook=True,
              heading="An example Graph for" + ,directed=True)
 
colors=[]
for i in range(1,11):  
  c = generate_color()
  while(c in colors):
      c = generate_color()
  colors.append(c)
 
  val = generate_size_node()
  b = random.randint(3,5)
 
  g_complete.add_node(i,label=str(i),color=c,value=val,
                      title="Hello! I am Node "+str(i),borderWidth=b)
 
i=0
chosen_set = []
while(i!=20):
  eg = generate_edge()
  if(eg[0]!=eg[1] and not (eg in chosen_set)):
      chosen_set.append(eg)
      g_complete.add_edge(eg[0],eg[1])
      i+=1
 
g_complete.show_buttons(['physics'])
 
g_complete.show('A_Complete_Networkx_Graph.html')
display(HTML('A_Complete_Networkx_Graph.html'))