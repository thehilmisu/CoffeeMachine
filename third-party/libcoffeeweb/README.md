# libcoffeeweb

## Description

Library that provides recipes for different coffee-variants via a simple interface.
The recipes are provided in a JSON format described in the [JSON format](#json-format)
section.

## Library Usage

**Usage Example:**
```cpp
// Simple usage example, within a fictional application object

// App constructor
MyApp::MyApp(QObject *parent)
    : QObject(parent)
    , coffeeWebLib_(new CoffeeWeb(this))
{
    // Connect reply signal of coffee web lib
    connect(coffeeWebLib_, &CoffeeWeb::recipesRequestReply, this, &MyApp::recipesReply);
}

// (private) MyApp method that requests a recipes list..
void MyApp::requestRecipes()
{
  lastReqId_ = coffeWebLib_->requestRecipes(); // lastReqId_ :
}

// (private) MyApp method that 'receives' request replies
void MyApp::recipesReply(quint32 id, const QString& recipesJson)
{
  // Evaluate reply...
  if (id != lastReqId_) return;

  // TODO evaluate JSON...
}


```


## JSON format

The library provides coffee recipes from different sources in the background as
a simple JSON file. The file is structured as follows: (Even though it is not
valid JSON, the following description contains code comments to describe and
clarify properties)

```javascript
{
  "return_code" : 200, // A return code (200 = OK, 408 = Request timed out),
                       // for codes != 200 the  error_message is set and all other values are undefined
  "error_message": "",


  "collection_name": "This is a recipe collection name", // name of the recipe collection
  "collection_version": "0.2.21", // version of the recipe collection (following semver)

  "recipes": [ // list of recipes
    { // A recipe object has the following properties:
      "name": "Espresso",
      "beans_g": 10,   // coffee beans in gram
      "grind_level": "medium", // grind level is one of:
                               // "extra-course", "course", "medium-coarse",
                               // "medium", "medium-fine", "fine", "extra-fine"
      "water_ml": 35,    // water for recipe in ml
      "water_temp": 95,  // water temperature for recipe

      "milk": {    // optional settings for milk, if used in recipe
          "milk_ml": 50,    // amount of milk in ml
          "milk_temp": 80,  // milk temperature
          "foam_ml": 10     // amount of foam to make in ml
      }
    }
  ]
}
```