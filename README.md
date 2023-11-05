# Cropper 

## Description

This small project was inspired by an article (the code is actually pretty similar), by [@ilyakava](https://artsy.github.io/blog/2014/09/24/using-pattern-recognition-to-automatically-crop-framed-art/).
It doesn't work that good, I haven't tried to improve it in any way.
Though, I will probably try in the future.

## Examples

```bash
"PATH/TO/EXECUTABLE" --input=input/00.jpg --ouput=output/00.jpg 
"PATH/TO/EXECUTABLE" --input=input/01.jpg --ouput=output/01.jpg 
"PATH/TO/EXECUTABLE" --input=input/02.jpg --ouput=output/02.jpg 
```

## Results

We can see that there is an issue in what it is considered a frame or not.
If we perform multiple iterations on [00.jpg](input/00.jpg) then we can see how the nested frames get cropped step by step.

<table>
  <tr>
    <td valign="top"> <img src="input/00.jpg" alt="image">
    <td valign="top">  <img src="output/00.jpg" alt="image">
    <td valign="top">  <img src="output/02.jpg" alt="image">
    <td valign="top">  <img src="output/03.jpg" alt="image">
  </tr>
</table>
