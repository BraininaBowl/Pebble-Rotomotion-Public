module.exports = [
	{ 
    "type": "heading", 
    "defaultValue": "ROTOMOTION" 
  }, 
{
  "type": "section",
  "items": [
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
      "label": "Ribbons", 
      "value": "3" 
    },
	 { 
      "label": "Flat", 
      "value": "0" 
    }
  ]
},
	  {
		  "type": "submit",
		  "defaultValue": "Save"
		}
  ]
}
	
];