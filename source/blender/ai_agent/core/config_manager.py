"""
Configuration Manager for AI Agent Suite
======================================

Handles secure storage and retrieval of API keys and other configuration
settings for the AI Agent Suite.
"""

import os
import json
import keyring
from pathlib import Path
from typing import Dict, Any, Optional
import bpy


class ConfigManager:
    """Manages configuration settings for the AI Agent Suite."""
    
    # Service name for keyring storage
    SERVICE_NAME = "blender_ai_agent_suite"
    
    # Default configuration
    DEFAULT_CONFIG = {
        "gemini": {
            "api_key": "",
            "model": "gemini-pro",
            "temperature": 0.7,
            "max_tokens": 2048,
        },
        "athena_mist": {
            "api_key": "",
            "endpoint": "https://api.athenamist.ai/v1",
        },
        "ui": {
            "theme": "dark",
            "font_size": 12,
            "show_advanced": False,
        },
    }
    
    def __init__(self):
        self.config_dir = Path(bpy.utils.user_resource('CONFIG', path='scripts', create=True))
        self.config_file = self.config_dir / "ai_agent_config.json"
        self.config = self._load_config()
    
    def _load_config(self) -> Dict[str, Any]:
        """Load configuration from file or create default."""
        try:
            if self.config_file.exists():
                with open(self.config_file, 'r') as f:
                    config = json.load(f)
                    # Merge with default config to ensure all keys exist
                    return self._merge_dicts(self.DEFAULT_CONFIG, config)
        except Exception as e:
            print(f"Error loading config: {e}")
        
        # Return default config if loading fails
        return self.DEFAULT_CONFIG.copy()
    
    def save_config(self) -> bool:
        """Save current configuration to file."""
        try:
            self.config_dir.mkdir(parents=True, exist_ok=True)
            with open(self.config_file, 'w') as f:
                json.dump(self.config, f, indent=2)
            return True
        except Exception as e:
            print(f"Error saving config: {e}")
            return False
    
    @staticmethod
    def _merge_dicts(base: Dict[Any, Any], update: Dict[Any, Any]) -> Dict[Any, Any]:
        """Recursively merge two dictionaries."""
        result = base.copy()
        for key, value in update.items():
            if key in result and isinstance(result[key], dict) and isinstance(value, dict):
                result[key] = ConfigManager._merge_dicts(result[key], value)
            else:
                result[key] = value
        return result
    
    # API Key Management
    
    def get_api_key(self, service: str) -> str:
        """Get API key from secure storage."""
        try:
            # First try to get from keyring
            key = keyring.get_password(self.SERVICE_NAME, f"{service}_api_key")
            if key:
                return key
                
            # Fallback to config file
            return self.config.get(service, {}).get("api_key", "")
        except Exception as e:
            print(f"Error getting API key: {e}")
            return ""
    
    def set_api_key(self, service: str, key: str, save_to_keyring: bool = True) -> bool:
        """Store API key securely."""
        try:
            if save_to_keyring:
                keyring.set_password(self.SERVICE_NAME, f"{service}_api_key", key)
            
            # Also store in config (in memory, not saved to disk)
            if service not in self.config:
                self.config[service] = {}
            self.config[service]["api_key"] = key
            
            return True
        except Exception as e:
            print(f"Error setting API key: {e}")
            return False
    
    def delete_api_key(self, service: str) -> bool:
        """Remove API key from secure storage."""
        try:
            # Remove from keyring
            keyring.delete_password(self.SERVICE_NAME, f"{service}_api_key")
            
            # Remove from config
            if service in self.config:
                if "api_key" in self.config[service]:
                    del self.config[service]["api_key"]
            
            return True
        except Exception as e:
            print(f"Error deleting API key: {e}")
            return False
    
    # Configuration accessors
    
    def get_setting(self, section: str, key: str, default: Any = None) -> Any:
        """Get a configuration setting."""
        return self.config.get(section, {}).get(key, default)
    
    def set_setting(self, section: str, key: str, value: Any) -> None:
        """Set a configuration setting."""
        if section not in self.config:
            self.config[section] = {}
        self.config[section][key] = value
    
    # Singleton pattern
    _instance = None
    
    @classmethod
    def get_instance(cls) -> 'ConfigManager':
        """Get the singleton instance of ConfigManager."""
        if cls._instance is None:
            cls._instance = ConfigManager()
        return cls._instance
