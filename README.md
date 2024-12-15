Spellcheck program based off the Trie dataset

Walkthrough of the program:
Upon booting the app, you'll find two options, one is a spellchecker for individual words
and the other is a batch spellchecker.

Instructions to run the program:
  You can use the exe included in the github to test the code and follow the walkthrough below

  The individual word option will allow you to see how the checker works in terms of taking an misspelled word and suggesting up to 5 options that the word may be.
  You can also select the word you meant and skip over if you don't see the word you meant. The purpose of this functionality is to be able to 
  select the option that will replace your incorrect word. You are then returned to the initial word prompt.

  The next option is batch spell checking. This is intended to take a file with text in it and run it through the spellchecker one word at a time.
  This is to demonstrate the programs ability to take multiple words at once and correctly return the proper words instead of any typos that may
  be present. 

Some problems I had with this code in the early stages:
  I had a great deal of problems with many unrelated words that I've almost never heard of being recommended. One solution of course would have been to grab a different
  word set to use for the database, but I felt that this would be reducing how many words overall I had available for searches. This would reduce the effectiveness of the code.
  I instead took a second file that was a list of common words in the english language. I put this code up against the initial dictionary database and placed
  a higher priority on words in the common text file. This way any obscure words stay in possibility still but common words are more at the forefront 
  depending on your input of course.

  I also had an issue with getting this on a webpage. Unfortunately I was unable to get this to work to the best degree I wanted to but I am happy with the product.

  I also had great difficulty with the distance function to compare how close a word was with another. In the end it made a lot more sense but starting out it was certainly a lot
  tougher than I thought initially. There was a big issue with ensuring I had the right indices in the for loops so a lot of troubleshooting was required.
