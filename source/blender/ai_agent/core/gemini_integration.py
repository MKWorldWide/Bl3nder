"""
Gemini AI Integration for Blender AI Agent
=========================================

This module integrates Google's Gemini AI capabilities with Blender, providing
advanced natural language understanding and 3D modeling assistance.
"""

import os
import json
import asyncio
import aiohttp
import logging
from typing import Dict, List, Any, Optional, Tuple, Union
from dataclasses import dataclass, asdict
from enum import Enum
import bpy
from mathutils import Vector, Matrix

from .config_manager import ConfigManager


class GeminiMode(Enum):
    """Gemini interaction modes for different types of assistance."""
    MODELING_ASSISTANT = "modeling_assistant"  # General 3D modeling assistance
    CODE_HELPER = "code_helper"               # Python/API code generation
    SCENE_ANALYSIS = "scene_analysis"         # Analyze and suggest scene improvements
    WORKFLOW_OPTIMIZER = "workflow_optimizer"  # Optimize modeling workflow
    TUTORIAL_MODE = "tutorial_mode"           # Step-by-step guidance
    
    @classmethod
    def get_mode_description(cls, mode: 'GeminiMode') -> str:
        """Get a user-friendly description for a mode."""
        descriptions = {
            cls.MODELING_ASSISTANT: "Get help with 3D modeling tasks",
            cls.CODE_HELPER: "Generate and debug Python/Blender API code",
            cls.SCENE_ANALYSIS: "Analyze and optimize your 3D scene",
            cls.WORKFLOW_OPTIMIZER: "Improve your Blender workflow",
            cls.TUTORIAL_MODE: "Step-by-step guidance and tutorials"
        }
        return descriptions.get(mode, "AI Assistant")


@dataclass
class GeminiResponse:
    """Response from Gemini AI."""
    content: str
    confidence: float
    suggestions: List[str]
    actions: List[Dict[str, Any]]
    context: Dict[str, Any]
    mode: GeminiMode


class GeminiIntegration:
    """
    Google Gemini AI integration for Blender.
    
    Provides advanced AI capabilities including:
    - Natural language understanding for 3D modeling
    - Code generation and explanation
    - Scene analysis and suggestions
    - Workflow optimization
    - Interactive tutorials
    """
    
    # API endpoints
    BASE_URL = "https://generativelanguage.googleapis.com/v1beta"
    GENERATE_CONTENT_ENDPOINT = "{model}:generateContent"
    STREAM_GENERATE_CONTENT_ENDPOINT = "{model}:streamGenerateContent"
    
    # Default model to use
    DEFAULT_MODEL = "gemini-pro"
    
    def __init__(self, api_key: str = None, mode: Union[GeminiMode, str] = GeminiMode.MODELING_ASSISTANT):
        """Initialize the Gemini integration.
        
        Args:
            api_key: Google Gemini API key (optional, will use config if not provided)
            mode: Initial interaction mode (can be string or GeminiMode)
        """
        self.config = ConfigManager.get_instance()
        self.api_key = api_key or self.config.get_api_key("gemini")
        self.mode = mode if isinstance(mode, GeminiMode) else GeminiMode(mode)
        self.session = None
        self.conversation_history = []
        self.last_response = None
        self._initialized = False
        self.logger = self._setup_logger()
    
    def _setup_logger(self) -> logging.Logger:
        """Set up a logger for this module."""
        logger = logging.getLogger("gemini_integration")
        logger.setLevel(logging.INFO)
        
        # Create console handler with a higher log level
        ch = logging.StreamHandler()
        ch.setLevel(logging.INFO)
        
        # Create formatter and add it to the handlers
        formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        ch.setFormatter(formatter)
        
        # Add the handler to the logger
        logger.addHandler(ch)
        
        return logger
    
    async def initialize(self) -> bool:
        """Initialize the Gemini integration.
        
        Returns:
            bool: True if initialization was successful, False otherwise
        """
        if not self.api_key:
            self.logger.error("No Gemini API key provided")
            return False
            
        try:
            self.session = aiohttp.ClientSession(
                timeout=aiohttp.ClientTimeout(total=60),
                headers={
                    "Content-Type": "application/json",
                    "x-goog-api-key": self.api_key
                }
            )
            self._initialized = True
            self.logger.info("Gemini integration initialized successfully")
            return True
        except Exception as e:
            self.logger.error(f"Failed to initialize Gemini integration: {str(e)}")
            return False
        
    async def close(self):
        """Clean up resources."""
        try:
            if self.session:
                await self.session.close()
                self.session = None
            self._initialized = False
            self.logger.info("Gemini integration closed")
        except Exception as e:
            self.logger.error(f"Error closing Gemini integration: {str(e)}")
    
    def _get_model_endpoint(self, model: str = None, stream: bool = False) -> str:
        """Get the appropriate API endpoint for the given model.
        
        Args:
            model: The model name (default: self.DEFAULT_MODEL)
            stream: Whether to use the streaming endpoint
            
        Returns:
            str: The full API endpoint URL
        """
        model = model or self.DEFAULT_MODEL
        endpoint = self.STREAM_GENERATE_CONTENT_ENDPOINT if stream else self.GENERATE_CONTENT_ENDPOINT
        return f"{self.BASE_URL}/models/{endpoint.format(model=model)}"
    
    def _build_prompt(self, user_input: str, context: Dict[str, Any] = None) -> Dict[str, Any]:
        """Build the prompt for the Gemini API.
        
        Args:
            user_input: The user's input text
            context: Additional context for the prompt
            
        Returns:
            Dict containing the structured prompt for the API
        """
        # Get configuration
        temperature = self.config.get_setting("gemini", "temperature", 0.7)
        max_tokens = self.config.get_setting("gemini", "max_tokens", 2048)
        
        # Build system message based on mode
        mode_description = GeminiMode.get_mode_description(self.mode)
        system_message = f"""You are a helpful AI assistant integrated into Blender 3D. 
        Current Mode: {self.mode.value} - {mode_description}
        
        You help users with 3D modeling, animation, and rendering tasks. 
        Be concise, accurate, and focus on practical solutions."""
        
        # Start with system message
        messages = [{
            "role": "user",
            "parts": [{"text": system_message}]
        }]
        
        # Add context about the current Blender scene
        scene_info = self._get_scene_info()
        context_message = {
            "role": "user",
            "parts": [{"text": f"Current Blender context: {scene_info}"}]
        }
        messages.append(context_message)
        
        # Add any additional context
        if context:
            context_str = json.dumps(context, indent=2)
            messages.append({
                "role": "user",
                "parts": [{"text": f"Additional context: {context_str}"}]
            })
        
        # Add conversation history
        for user_msg, assistant_resp in self.conversation_history[-5:]:  # Keep last 5 exchanges
            messages.extend([
                {"role": "user", "parts": [{"text": user_msg}]},
                {"role": "model", "parts": [{"text": assistant_resp.content}]}
            ])
        
        # Add current user input
        messages.append({
            "role": "user",
            "parts": [{"text": user_input}]
        })
        
        return {
            "contents": messages,
            "generationConfig": {
                "temperature": temperature,
                "topK": 40,
                "topP": 0.95,
                "maxOutputTokens": max_tokens,
                "stopSequences": ["###"]
            },
            "safetySettings": [
                {
                    "category": "HARM_CATEGORY_HARASSMENT",
                    "threshold": "BLOCK_MEDIUM_AND_ABOVE"
                },
                {
                    "category": "HARM_CATEGORY_HATE_SPEECH",
                    "threshold": "BLOCK_MEDIUM_AND_ABOVE"
                },
                {
                    "category": "HARM_CATEGORY_SEXUALLY_EXPLICIT",
                    "threshold": "BLOCK_MEDIUM_AND_ABOVE"
                },
                {
                    "category": "HARM_CATEGORY_DANGEROUS_CONTENT",
                    "threshold": "BLOCK_MEDIUM_AND_ABOVE"
                }
            ]
        }
    
    def _get_scene_info(self) -> Dict[str, Any]:
        """Get information about the current Blender scene."""
        scene = bpy.context.scene
        return {
            "active_object": scene.objects.active.name if scene.objects.active else None,
            "object_count": len(scene.objects),
            "mode": bpy.context.mode,
            "selected_objects": [obj.name for obj in bpy.context.selected_objects],
            "render_engine": scene.render.engine,
            "frame_current": scene.frame_current,
        }
    
    async def process_query(
        self, 
        query: str, 
        context: Dict[str, Any] = None,
        stream: bool = False,
        **kwargs
    ) -> GeminiResponse:
        """Process a user query using the Gemini API.
        
        Args:
            query: The user's input text
            context: Additional context for the query
            stream: Whether to use streaming responses
            **kwargs: Additional parameters to pass to the API
            
        Returns:
            GeminiResponse containing the AI's response
        """
        if not self._initialized and not await self.initialize():
            return self._create_error_response("Failed to initialize Gemini integration", context)
        
        try:
            # Build the API request
            prompt = self._build_prompt(query, context)
            model = kwargs.get("model") or self.DEFAULT_MODEL
            
            # Add any additional parameters
            if "temperature" not in kwargs:
                kwargs["temperature"] = self.config.get_setting("gemini", "temperature", 0.7)
            
            # Update prompt with any additional parameters
            if "generationConfig" not in prompt:
                prompt["generationConfig"] = {}
            
            for key, value in kwargs.items():
                if key in ["temperature", "topP", "topK", "maxOutputTokens"]:
                    prompt["generationConfig"][key] = value
            
            # Make the API request
            url = f"{self._get_model_endpoint(model, stream=stream)}?key={self.api_key}"
            
            async with self.session.post(url, json=prompt) as response:
                if response.status != 200:
                    error_text = await response.text()
                    self.logger.error(f"API Error {response.status}: {error_text}")
                    return self._create_error_response(
                        f"API Error: {response.status} - {error_text}",
                        context
                    )
                
                if stream:
                    return await self._handle_streaming_response(response, context)
                else:
                    return await self._handle_standard_response(response, query, context)
                
        except asyncio.TimeoutError:
            self.logger.error("Request to Gemini API timed out")
            return self._create_error_response("Request timed out. Please try again.", context)
            
        except Exception as e:
            self.logger.exception("Error processing query")
            return self._create_error_response(
                f"An error occurred while processing your request: {str(e)}",
                context
            )
    
    async def _handle_standard_response(
        self, 
        response: aiohttp.ClientResponse,
        query: str,
        context: Dict[str, Any]
    ) -> GeminiResponse:
        """Handle a standard (non-streaming) API response."""
        result = await response.json()
        
        # Extract the response text
        if 'candidates' in result and len(result['candidates']) > 0:
            candidate = result['candidates'][0]
            content_parts = candidate.get('content', {}).get('parts', [])
            content = "\n".join(part.get('text', '') for part in content_parts)
            
            # Check for safety ratings
            safety_ratings = candidate.get('safetyRatings', [])
            if any(r.get('blocked', False) for r in safety_ratings):
                blocked_categories = [
                    r['category'] for r in safety_ratings 
                    if r.get('blocked', False)
                ]
                content = (
                    "I'm sorry, but I can't provide a response to that query "
                    f"due to content safety restrictions ({', '.join(blocked_categories)})."
                )
        else:
            content = "I'm sorry, I couldn't generate a response. Please try again."
        
        # Create response object
        response_obj = GeminiResponse(
            content=content,
            confidence=0.9,  # Could be based on response metadata
            suggestions=self._extract_suggestions(content),
            actions=self._extract_actions(content),
            context=context or {},
            mode=self.mode
        )
        
        # Update conversation history
        if query and content:
            self.conversation_history.append((query, response_obj))
        
        self.last_response = response_obj
        return response_obj
    
    async def _handle_streaming_response(
        self, 
        response: aiohttp.ClientResponse,
        context: Dict[str, Any]
    ) -> GeminiResponse:
        """Handle a streaming API response."""
        content_parts = []
        
        async for line in response.content:
            if line:
                try:
                    # Parse the server-sent event
                    if line.startswith(b'data: '):
                        data = json.loads(line[6:])  # Remove 'data: ' prefix
                        
                        # Check for errors
                        if 'error' in data:
                            error = data['error']
                            return self._create_error_response(
                                f"API Error: {error.get('message', 'Unknown error')}",
                                context
                            )
                        
                        # Extract content
                        candidates = data.get('candidates', [])
                        if candidates:
                            parts = candidates[0].get('content', {}).get('parts', [])
                            for part in parts:
                                if 'text' in part:
                                    content_parts.append(part['text'])
                
                except json.JSONDecodeError:
                    self.logger.warning("Failed to parse streaming response")
        
        # Create response from all collected parts
        content = "".join(content_parts).strip()
        
        response_obj = GeminiResponse(
            content=content,
            confidence=0.9,
            suggestions=[],
            actions=[],
            context=context or {},
            mode=self.mode
        )
        
        self.last_response = response_obj
        return response_obj
    
    def _extract_suggestions(self, content: str) -> List[str]:
        """Extract suggested follow-up questions from the response."""
        # This is a simple implementation - could be enhanced with more sophisticated parsing
        suggestions = []
        
        # Look for patterns like "You might want to ask..."
        if "you might want to" in content.lower() or "you could try" in content.lower():
            # Simple heuristic: take the last 2-3 sentences as suggestions
            sentences = [s.strip() for s in content.split('.') if s.strip()]
            if len(sentences) > 1:
                suggestions = [s for s in sentences[-3:] if not s.startswith((" ", "\n"))]
        
        return suggestions
    
    def _extract_actions(self, content: str) -> List[Dict[str, Any]]:
        """Extract potential actions from the response."""
        # This is a placeholder - could be enhanced with more sophisticated parsing
        actions = []
        
        # Simple heuristic: look for code blocks
        if "```" in content:
            actions.append({
                "type": "code_execution",
                "description": "Execute the provided code",
                "code": content.split("```")[1].strip()
            })
        
        return actions
    
    def _create_error_response(self, message: str, context: Dict[str, Any] = None) -> GeminiResponse:
        """Create an error response."""
        return GeminiResponse(
            content=message,
            confidence=0.0,
            suggestions=[],
            actions=[],
            context=context or {},
            mode=self.mode
        )
    
    def set_mode(self, mode: Union[GeminiMode, str]):
        """Set the interaction mode."""
        if isinstance(mode, str):
            mode = GeminiMode(mode)
        self.mode = mode
        return f"Mode set to: {mode.value}"


# Example usage
if __name__ == "__main__":
    async def test_conversation():
        # Initialize with your API key
        gemini = GeminiIntegration(api_key="YOUR_API_KEY")
        await gemini.initialize()
        
        # Example query
        response = await gemini.process_query("How do I add a cube to the scene?")
        print("Response:", response.content)
        
        # Clean up
        await gemini.close()
    
    # Run the test
    asyncio.run(test_conversation())
