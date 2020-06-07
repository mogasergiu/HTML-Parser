# HTML-Parser
  Program that reads HTML files with internal or inline CSS and Javascript and beautifies the text with proper indentations and spacing,
while also offering various editing abilities through the CLI, similar to those offered by some JavaScript methods, such as modifying any
element’s (chosen by CSS selector) attributes list or inner text. (implemented using MultiWay Tree).

  The tags are given ID's depending on their position in the HTML tree.< br/>
*example:*<br />
tag1 - ID 0
tag2 - ID 1
  tag21 - ID 1.1
  tag22 - ID 1.2
    tag221 - ID 1.2.1
  tag23 - ID 1.3
tag3 - ID 2

  In order to run the program three parameters in the command line must be given (./HPAR arg1 arg2 arg3):
  - a HTML file containing the code that the user wants to beautify
  - a file with commands that the user wants the program to execute
  - an output file that represents the result of the commands executed on the HTML file
  
  **Available commands:**<br />
   - format: beautifies the code
   - add ID=<parent_ID> tagHTML="<HTML_tag>": adds the specified tag as the child of the tag with the specified ID
   - deleteRecursively selector="<CSS_selector>": deletes the elements with the specified CSS selector and all of its children elements
   - overrideStyle selector="<CSS_selector>" style="<new_style>": overwrites the style attribute of the elements with the specified CSS selector with the new specified style
   - appendStyle selector="<CSS_selector>" style="<new_style>": the same as overrideStyle, but instead of overwriting it appends the new style
   
  An example of file arguments are given in the "example" folder.
