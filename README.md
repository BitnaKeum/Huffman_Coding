# Huffman_Coding
Huffman Encoder & Decoder
<br><br><br>

<hr><br>
<h1> Encoder </h1>
 From input.txt, make Huffman Table<br>
 Using Huffman Table, encode input.txt and save to Huffman_code.hbs<br>
 At last, encode Huffman Table and save to Huffman_table.hbs<br>
 <br>

<h4>input.txt</h4>

![image](https://user-images.githubusercontent.com/37769713/99864959-c8225680-2be9-11eb-9d6b-9c9d2ff60941.png)

<br>

<h4>Huffman Table</h4>
 ASCII code  -  Frequency  -  Codeword <br>

 ![image](https://user-images.githubusercontent.com/37769713/99864091-d28e2180-2be4-11eb-8de5-f023811da87f.png)
![image](https://user-images.githubusercontent.com/37769713/99864098-dcb02000-2be4-11eb-9fb9-c42c593ed6d8.png)
<br><br>

<h4>encoding string</h4>

![image](https://user-images.githubusercontent.com/37769713/99864132-0cf7be80-2be5-11eb-8aaf-1796fb7dcc6e.png)

...

![image](https://user-images.githubusercontent.com/37769713/99864226-868fac80-2be5-11eb-917a-c6377497e7c4.png)

EOD(1011001100100) + Stuffing bit(00000) is inserted to the end of the encoding string.
<br><br>

<h4>Huffman_code.hbs</h4>

![image](https://user-images.githubusercontent.com/37769713/99864303-d1112900-2be5-11eb-9991-1ef00547a0ed.png)

...

![image](https://user-images.githubusercontent.com/37769713/99864316-e4bc8f80-2be5-11eb-8e61-317d48e7038d.png)

<br>

<h4>Huffman_table.hbs</h4>

![image](https://user-images.githubusercontent.com/37769713/99864346-10d81080-2be6-11eb-9907-1db870499121.png)

<br><br>
<hr><br>

<h1> Decoder </h1>
 From Huffman_table.hbs, remake Huffman Table<br>
 Using Huffman Table, decode Huffman_code.hbs and write to output.txt<br><br>

<h4>output.txt</h4>

![image](https://user-images.githubusercontent.com/37769713/99864459-c30fd800-2be6-11eb-8f33-f9e64651108c.png)

same with input.txt
<br><br>

