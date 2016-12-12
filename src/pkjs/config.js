module.exports = [
{ 
 "type": "heading", 
 "defaultValue": "ROTOMOTION" 
}, 
{
 "type": "section",
 "items": [
   {
      "type": "heading",
      "defaultValue": "Layout"
   },
   {
      "type": "toggle",
      "messageKey": "twelveHour",
      "label": "Twelve hour mode",
		"defaultValue": false
   },
	  {
      "type": "radiogroup",
      "messageKey": "shaderMode",
      "label": "3D effect",
		"defaultValue": "1",
      "options": 
        [
          { 
            "label": "Cylinder", 
            "value": "1"
          },
          { 
            "label": "Inverted cylinder", 
            "value": "2"
          },
          { 
            "label": "Banner", 
            "value": "3"
          },
          { 
            "label": "Frosted glass", 
            "value": "4"
          },
          { 
            "label": "Flat", 
            "value": "0"
          }
        ]
   }
]},
{
  "type": "section",
  "items": [
	    {
      "type": "heading",
      "defaultValue": "Colors"
    },
    {
      "type": "color",
      "messageKey": "BackgroundColor",
      "defaultValue": "0x000000",
      "sunlight": false,
      "label": "Background color"
    },
    {
      "type": "color",
      "messageKey": "HourColor",
      "defaultValue": "0xFFFFFF",
      "sunlight": false,
      "label": "Hours"
    },
    {
      "type": "color",
      "messageKey": "MinColor",
      "defaultValue": "0xFFFFFF",
      "sunlight": false,
      "label": "Minutes"
    },
    {
      "type": "color",
      "messageKey": "HourBgColor",
      "defaultValue": "0x000000",
      "sunlight": false,    
      "label": "Hours background"
    },
    {
      "type": "color",
      "messageKey": "MinBgColor",
      "defaultValue": "0x000000",
      "sunlight": false,
      "label": "Minutes background"
    },
	  {
      "type": "color",
      "messageKey": "ArrowColor",
      "defaultValue": "0xFF0000",
      "sunlight": false,
      "label": "Arrows"
    },
	  {
      "type": "toggle",
      "capabilities": ["COLOR"],
      "messageKey": "dropShadow",
      "label": "Edge shadows",
		"defaultValue": true
   }
  ]
  
},
	  {
		  "type": "submit",
		  "defaultValue": "Save"
		}
];