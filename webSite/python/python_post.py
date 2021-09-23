import cgi
import cgitb
cgitb.enable()

print 'Content-type: text/html'
print

print '<html><head><title>Test Python</title></head><body>'
form = cgi.FieldStorage()
if form.getvalue('user') != None:
    print "Hi "
    print form.getvalue('user')

print '</body>welcome to CGI Post Python</html>'
