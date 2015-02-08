#GraphKit  

A highly performant native graph data store for Node.js. Welcome to GraphKit. Written completely in C/C\+\+, GraphKit is woven into the fabric of Node.js. The power of graph data models may now be leveraged within the Node.js environment and can easily scale to complex networked systems. Classic mathematics is at the core of GraphKit, offering developers synthesized tools for machine learning and predictive analytics that expand the imagination of technology and science.

##Installation

npm install graphkit

##Documentation

Coming soon!

##A Taste

```javascript
// get some library classes
let gk = require('../');
let Entity = gk.Entity;
let Action = gk.Action;
let Graph = gk.Graph;

// create a new Entity Node that is of type User
let user = new Entity('User');

// add some properties
user['name'] = 'Daniel';
user['age'] = 31;

// add user to a subset in the Graph named developer
user.addGroup('developer');

// create a new Action Node that is of type Read
let read = new Action('Read');

// add some session details
read['sessionId'] = 123;

// create a new Entity Node that is of type Book
let book = new Entity('Book');
book['title'] = 'Learning GraphKit';

// create a relationship between the Entities using the Action
read.addSubject(user);
read.addObject(book);

// graph all the nodes
graph.insert(read);
```

##License

[AGPL v3.0](http://choosealicense.com/licenses/agpl-3.0/)

##Contributors

[Daniel Dahan](https://github.com/danieldahan)