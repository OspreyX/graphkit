
#GraphKit  

A highly performant native graph data store for Node.js. Welcome to GraphKit. Written completely in C/C\+\+, GraphKit is woven into the fabric of Node.js. The power of graph data models may now be leveraged within the Node.js environment and can easily scale to complex networked systems. Classic mathematics is at the core of GraphKit, offering developers synthesized tools for machine learning and predictive analytics that expand the imagination of technology and science.

##Installation

npm install graphkit

##Documentation

[graphkit.io](http://graphkit.io)

##A Taste

```javascript
1 // command: node --harmony filename
2  
3 'use strict';
4 'use esnext';
5  
6 // Require package Classes.
7 let gk = require('graphkit');
8 let Graph = gk.Graph;
9 let Entity = gk.Entity;
10 
11// Create a Graph instance.
12let g1 = new Graph();
13 
14// Create a User Entity.
15let u1 = new Entity("User");
16 
17// Give u1 some properties.
18u1["name"] = "Eve";
19u1["age"] = 26;
20 
21// Add u1 to a group. This creates a subset in the Graph named "female".
22u1.addGroup("Female");
23 
24// Output: 0, before inserting in the Graph.
25console.log(u1.id);
26 
27// Output: 1, after inserting in the Graph.
28g1.insert(u1);
29console.log(u1.id);
30 
31// Create a Book Entity using Graph factory methods.
32let b1 = g1.createEntity("Book");
33b1['title'] = 'Deep C Secrets';
34b1.addGroup('Thriller');
35 
36// Output: 1, b1 was automatically inserted in the Graph.
37console.log(b1.id);
38 
39// Create a Read Action using Graph factory methods.
40let a1 = g1.createAction('Read');
41 
42// Output: 1, a1 was automatically inserted in the Graph.
43console.log(a1.id);
44 
45// Record the session the Action occurred in.
46a1['session'] = 123;
47 
48// Insert u1 as a subject for the Action.
49a1.subjects().insert(u1);
50 
51// Create another Book Entity.
52let b2 = g1.createEntity('Book');
53b2['title'] = 'Mastering Node.js';
54b2.addGroup('Suspense');
55b2.addGroup('Favourite');
56 
57// Output: 2, b2 was automatically inserted in the Graph.
58console.log(b2.id);
59 
60// Add the books as the objects for the Action.
61a1.objects().insert(b1);
62a1.objects().insert(b2);
63 
64// Output: 1
65console.log(a1.subjects().size());
66 
67// Output: 2
68console.log(a1.objects().size());
69 
70// Output: Eve read 2 books.
71console.log(a1.subjects()[0]['name'], a1.type.toLowerCase(), a1.objects().size(), a1.objects()[0].type.toLowerCase() + 's.');
72 
73// Output: There is 1 user and 2 books in the graph.
74console.log('There is', g1.Entity.User.size(), g1.Entity.User.type.toLowerCase(), 'and', g1.Entity.Book.size(), g1.Entity.Book.type.toLowerCase() + 's', 'in the graph.');
```

##License

[AGPLv3](http://choosealicense.com/licenses/agpl-3.0/)

##Contributors

[Daniel Dahan](https://github.com/danieldahan)