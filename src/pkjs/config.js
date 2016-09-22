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
	"capabilities": ["RECT"],
  "options": [
    { 
      "label": "Cylinder", 
      "value": "1",
		"defaultValue": true
    },
    { 
      "label": "Inverted cylinder", 
      "value": "2" 
    },
    { 
      "label": "Ribbon", 
      "value": "3", 
    },
    { 
      "label": "Banner", 
      "value": "4"
    },	  
	 { 
      "label": "Flat", 
      "value": "0" 
    }
  ]
},
	  {
  "type": "radiogroup",
  "messageKey": "shaderMode",
  "label": "3D effect",
	"capabilities": ["ROUND"],
  "options": [
    { 
      "label": "Cylinder", 
      "value": "1",
		"defaultValue": true
    },
    { 
      "label": "Inverted cylinder", 
      "value": "2" 
    },
    { 
      "label": "Banner", 
      "value": "4"
    },	  
	 { 
      "label": "Flat", 
      "value": "0" 
    }
  ]
}
  ]
},
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
      "label": "Background Color"
    },
    {
      "type": "color",
      "messageKey": "ForegroundColor",
      "defaultValue": "0xFFFFFF",
      "label": "Text Color"
    },
	  {
      "type": "color",
      "messageKey": "ArrowColor",
      "defaultValue": "0xFF0000",
      "label": "Arrows Color"
    },
    {
      "type": "toggle",
      "messageKey": "invHours",
      "label": "Inverted hours",
		"defaultValue": false
    },
    {
      "type": "toggle",
      "messageKey": "invMin",
      "label": "Inverted minutes",
		"defaultValue": false
    },
    {
      "type": "toggle",
      "messageKey": "dropShadow",
      "label": "Edge shadows",
		"defaultValue": true,
		"capabilities": ["COLOR"]
    }
  ]
  
},
	  {
		  "type": "submit",
		  "defaultValue": "Save"
		}
];