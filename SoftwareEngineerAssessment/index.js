const Router = require('./router')
const urlLinks = [{ "name": "Facebook", "url": "https://www.facebook.com/ashwin.subu/" },{ "name": "LinkedIn", "url": "https://www.linkedin.com/in/ashwin-subramaniam/" },{ "name": "GitHub", "url": "https://github.com/ashwinsubu" }]
const iconLinks=["https://simpleicons.org/icons/facebook.svg", "https://simpleicons.org/icons/linkedin.svg", "https://simpleicons.org/icons/github.svg"]
addEventListener('fetch', event => {
  event.respondWith(handleRequest(event.request))
})
/**
 * Respond with hello worker text
 * @param {Request} request
 */
async function handleRequest(request) {
    const r = new Router()
    // Replace with the appropriate paths and handlers
    // r.get('.*/bar', () => new Response('responding for /bar'))
    r.get('.*/links', request => handle(request))
    // r.post('.*/foo.*', request => handler(request))
    // r.get('/demos/router/foo', request => fetch(request)) // return the response from the origin
	const init = {
        headers: { 'content-type': 'text/html;charset=UTF-8' },
    }
    var link="https://static-links-page.signalnerve.workers.dev"
	var res =  await fetch(link, init)

	res = rewriter.transform(res);
	res = rewriter1.transform(res)
	res = rewriter2.transform(res)
 	res = rewriter3.transform(res)
	res = rewriter4.transform(res)

	const results = await home(res)
	// const results = await home(rewriter1.transform(results1))
    r.get('/', () => new Response(results, init)) // return a default message for the root route

    const resp = await r.route(request)
    return resp
}

const rewriter = new HTMLRewriter()
	.on("div", new AttributeRewriter("id"))

const rewriter1 = new HTMLRewriter()
	.on("img", new ImageRewriter("id"))

const rewriter2 = new HTMLRewriter()
	.on("h1", new HOneRewriter("id"))

const rewriter3 = new HTMLRewriter()
	.on("title", new TitleRewriter())

const rewriter4 = new HTMLRewriter()
	.on("body", new BodyBGRewriter("class"))


function handle(request) {
    const init = {
        headers: { 'content-type': 'application/json' },
    }
    const body = JSON.stringify(urlLinks)

    return new Response(body, init)
}

async function home(res){
	return await res.text()
}

class AttributeRewriter {
  constructor(attributeName) {
    this.attributeName = attributeName
  }
  element(element) {
    const attribute = element.getAttribute(this.attributeName)
    if (attribute && attribute == "links") {
    	for( var i=0; i< urlLinks.length; i++){
	    	var obj =urlLinks[i]
			element.append("<a target=\'_blank\' href=\'"+ obj['url'] +" \' >"+obj['name']+"</a>", {html:Boolean})
			element.setAttribute("style","color:red")	 
    	}
    }
    if (attribute && attribute == "social") {
    	for( var i=0; i< urlLinks.length; i++){
	    	var obj =urlLinks[i]
			element.append("<a target=\'_blank\' href=\'"+ obj['url'] +" \' >" + "<svg width=\"32\" height=\'32\'> <image xlink:href=\'"+ iconLinks[i] +"\' width=\"32\" height=\"32\"/>  </svg> </a> ", {html:Boolean})
    	}
    }

    if(attribute && attribute == "profile" || attribute == "social") {
    	element.setAttribute("style","display:inherit")	
    }
    
  }
}
class ImageRewriter{
	constructor(attributeName) {
    this.attributeName = attributeName
  }
  element(element) {
 	const attribute = element.getAttribute(this.attributeName)
    if (attribute && attribute == "avatar") {
    	element.setAttribute("src","https://avatars2.githubusercontent.com/u/53012400")
  	}
  }
}

class HOneRewriter{
	constructor(attributeName) {
    	this.attributeName = attributeName
  	}
  element(element) {
  	 const attribute = element.getAttribute(this.attributeName)
    if (attribute && attribute == "name") {
    	element.append("ashwinsubu");
  	}
  }
}

class TitleRewriter{
  element(element) {
    element.setInnerContent("Ashwin Subramaniam");
  }
}

class BodyBGRewriter{
	constructor(attributeName) {
    	this.attributeName = attributeName
    	console.log("console class")
  	}
	element(element) {
		console.log("asdasdasdasd")
    	const attribute = element.getAttribute(this.attributeName)
	   	element.setAttribute("style","background-image: linear-gradient(blue, white)")
  }
}

