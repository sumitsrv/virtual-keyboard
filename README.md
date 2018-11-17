Virtual Keyboard
==

The project was the resultant of my Master's theses. It aimed at providing a computer vision based keyboard that could help solve problems of portable, multi-lingual, custom keyboards. Early literature review reflected a few projects/papers looking out for the same but have had their own share of challenges.

Docs pertaining to the project:
==
  Research paper
  = 
  https://www.researchgate.net/publication/262370366_Real_Time_Mono-vision_Based_Customizable_Virtual_Keyboard_Using_Finger_Tip_Speed_Analysis
  Theses
  = 
  https://www.researchgate.net/publication/326190752_Mobile_devices_based_virtual_netbook
  
Note: The project code is currently being revamped to meet coding guidelines. 
== 

Usage
==
  The project is divided into multiple parts:
    Key detection:
      Build the project first as follows,
      $ make
      Execute the project using following command,
      $ ./vk <source_location_for_keyboard_image> <1/2>
      
      Second parameter of above command is to select live camera or existing image.
      If option 1 is selected: Live camera feed is captured and rendered to the user. As the user presses Escape key, the frame is grabbed and stored at the location provided in first parameter.
      If option 2 is selected: The image from location provided in first parameter is used as keyboard image.
      
   Hand Detection:
      The code is being fixed and should be available soon.
      
  
