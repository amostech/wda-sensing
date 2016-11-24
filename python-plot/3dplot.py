import pylab as p
from numpy import vstack,array
from numpy.random import rand
from scipy.cluster.vq import kmeans,vq
import mpl_toolkits.mplot3d.axes3d as p3

# data generation
x = []  #vstack((rand(150,2) + array([.5,.5]),rand(150,2)))
y = []
z = []

file = open('3d-experiment-with-toy-track.csv', 'r')
#file = open('data.csv', 'r')

for line in file:
    arr=line.replace("\r\n","").split(",")
    x.append(float(arr[0]))
    y.append(float(arr[1]))
    z.append(float(arr[2]))

#plot(x,y,'or')
#show()

#print x
#print y
#print z

fig=p.figure()

ax = p3.Axes3D(fig)
# scatter3D requires a 1D array for x, y, and z
# ravel() converts the 100x100 array into a 1x10000 array
ax.scatter3D(x,y,z)
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')
p.show()

'''
data = vstack(x,y)
# computing K-Means with K = 2 (2 clusters)
centroids,_ = kmeans(data,2)
# assign each sample to a cluster
idx,_ = vq(data,centroids)

# some plotting using numpy's logical indexing
plot(data[idx==0,0],data[idx==0,1],'ob',
     data[idx==1,0],data[idx==1,1],'or')
#plot(x,y, 'or')
plot(centroids[:,0],centroids[:,1],'sg',markersize=8)
show()
'''