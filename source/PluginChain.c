//
//  PluginChain.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PluginChain.h"
#include "EventLogger.h"

PluginChain newPluginChain(void) {
  PluginChain pluginChain = malloc(sizeof(PluginChainMembers));

  pluginChain->numPlugins = 0;
  pluginChain->plugins = malloc(sizeof(Plugin) * MAX_PLUGINS);

  return pluginChain;
}

static boolean _addPluginToChain(PluginChain pluginChain, Plugin plugin) {
  if(pluginChain->numPlugins + 1 >= MAX_PLUGINS) {
    logError("Could not add plugin, maximum number reached");
    return false;
  }
  else {
    pluginChain->plugins[pluginChain->numPlugins] = plugin;
    pluginChain->numPlugins++;
    return true;
  }
}

void addPluginsFromArgumentString(PluginChain pluginChain, const CharString argumentString) {
  // Expect a comma-separated string of plugins with colon separators for preset name
  // Example: plugin1:preset1name,plugin2:preset2name
  char* substringStart = argumentString->data;
  char* comma = strchr(argumentString->data, ',');
  char* endChar = argumentString->data + strlen(argumentString->data);
  CharString nameBuffer = newCharString();

  do {
    size_t substringLength;
    if(comma == NULL) {
      substringLength = strlen(argumentString->data);
    }
    else {
      substringLength = comma - substringStart;      
    }
    strncpy(nameBuffer->data, substringStart, substringLength);

    // TODO: Use colon as a separator for presets to load into these plugins
    PluginType pluginType = guessPluginType(nameBuffer);
    if(pluginType != PLUGIN_TYPE_INVALID) {
      Plugin plugin = newPlugin(pluginType, nameBuffer);
      _addPluginToChain(pluginChain, plugin);
    }

    if(comma == NULL) {
      break;
    }
    else {
      substringStart = comma + 1;
      comma = strchr(comma + 1, ',');
    }
  } while(substringStart < endChar);

  freeCharString(nameBuffer);
}

void initializePluginChain(PluginChain pluginChain) {
  for(int i = 0; i < pluginChain->numPlugins; i++) {
    Plugin plugin = pluginChain->plugins[i];
    plugin->open(plugin);
  }
}

void displayPluginInfo(PluginChain pluginChain) {
  for(int i = 0; i < pluginChain->numPlugins; i++) {
    Plugin plugin = pluginChain->plugins[i];
    plugin->displayPluginInfo(plugin);
  }
}

void process(PluginChain pluginChain, SampleBuffer inBuffer, SampleBuffer outBuffer) {
  
}

void freePluginChain(PluginChain pluginChain) {
  for(int i = 0; i < pluginChain->numPlugins; i++) {
    Plugin plugin = pluginChain->plugins[i];
    logInfo("Closing plugin '%s'", plugin->pluginName->data);
    freePlugin(plugin);
  }
  free(pluginChain);
}