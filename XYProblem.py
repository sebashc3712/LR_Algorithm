#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Nov 26 23:37:34 2019

@author: sebashc
"""
import pandas as pd
import matplotlib.pyplot as plt

n_c=32; # Here the numbers of customers must be updated

df=pd.read_csv('PrintProblem.csv',sep=',')
df=df[['X','Y']]

df1=df.iloc[:1,:]

df2=df.iloc[1:(n_c+1),:]

df3=df.iloc[(n_c+1):,:] 

_=plt.figure()
_=plt.scatter(df1['X'],df1['Y'],color='red',marker='*', alpha=1, label='Depot')
_=plt.scatter(df2['X'],df2['Y'],color='blue',marker='+', alpha=1, label='Customer')
_=plt.scatter(df3['X'],df3['Y'],color='orange',marker='o', alpha=1, label='Reloc. Point')

clusters=pd.read_csv('ClustersFile.csv',sep=',')
number_of_cluster=max(clusters['Cluster'])+1

df4=df1
count_clusters=0
for point in range(0,len(clusters)):
  if clusters.iloc[point,0]==count_clusters:
    df4=df4.append(pd.DataFrame({"X":[clusters.iloc[point,1]],
                             "Y":[clusters.iloc[point,2]]}))
  else:
    df4=df4.append(df1)
    _=plt.plot(df4['X'],df4['Y'], label='LKH Route')
    count_clusters+=1
    df4=df1
    df4=df4.append(pd.DataFrame({"X":[clusters.iloc[point,1]],
                             "Y":[clusters.iloc[point,2]]}))

df4=df4.append(df1)
_=plt.plot(df4['X'],df4['Y'], label='LKH Route') 

clusters2=pd.read_csv('RelocFile.csv',sep=',')

df4=df1
count_clusters=0
for point in range(0,len(clusters2)):
  if clusters2.iloc[point,0]==count_clusters:
    df4=df4.append(pd.DataFrame({"X":[clusters2.iloc[point,1]],
                             "Y":[clusters2.iloc[point,2]]}))
  else:
    df4=df4.append(df1)
    _=plt.plot(df4['X'],df4['Y'], linestyle='--', label='Reloc Route')
    count_clusters+=1
    df4=df1
    df4=df4.append(pd.DataFrame({"X":[clusters2.iloc[point,1]],
                             "Y":[clusters2.iloc[point,2]]}))

df4=df4.append(df1)
_=plt.plot(df4['X'],df4['Y'], linestyle='--', label='Reloc Route')
_=plt.xlabel('X')
_=plt.ylabel('Y')
_=plt.title('Relocated clusters after LKH')
_=plt.legend(loc='upper left')

plt.show()

df5=pd.read_csv('TrackingFile.csv')
_=plt.figure()
_=plt.plot(df5.values)
_=plt.ylabel('Objective Function Value')
_=plt.xlabel('Iteration')
_=plt.title('Objective Function over Iterations')
plt.show()

