import json
import re
import os
from pathlib import Path

def parse_vtt(vtt_path):
    """Parse VTT file and extract text content"""
    with open(vtt_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Remove VTT headers and metadata
    content = re.sub(r'WEBVTT.*?\n', '', content)
    content = re.sub(r'Kind:.*?\n', '', content)
    content = re.sub(r'Language:.*?\n', '', content)
    
    # Extract timestamp lines and text
    blocks = re.split(r'\n\n+', content)
    
    transcript_parts = []
    for block in blocks:
        lines = block.strip().split('\n')
        text_lines = []
        for line in lines:
            # Skip timestamp lines
            if '-->' in line:
                continue
            # Clean HTML tags and extra whitespace
            line = re.sub(r'<[^>]+>', '', line)
            line = line.strip()
            if line:
                text_lines.append(line)
        if text_lines:
            transcript_parts.append(' '.join(text_lines))
    
    return ' '.join(transcript_parts)

def extract_speakers_and_arguments(transcript, video_title):
    """Extract key speakers and arguments from transcript"""
    # This is a simplified extraction - in production would use NLP
    arguments = {
        'main_themes': [],
        'key_claims': [],
        'speakers_identified': []
    }
    
    # Common patterns for argument extraction
    sentences = re.split(r'[.!?]+', transcript)
    for sent in sentences:
        sent = sent.strip()
        if len(sent) > 50 and len(sent) < 300:
            # Look for argument indicators
            if any(word in sent.lower() for word in ['believe', 'think', 'argue', 'claim', 'point', 'issue', 'problem', 'toxic', 'masculinity', 'femininity', 'gender', 'rights', 'equality']):
                arguments['key_claims'].append(sent)
    
    # Limit to most relevant claims
    arguments['key_claims'] = arguments['key_claims'][:15]
    
    return arguments

# Video metadata
videos = [
    {
        'id': 'bVH_c-s9Oho',
        'title': "Men's Rights Activists vs Feminists | Middle Ground",
        'channel': 'Jubilee',
        'duration': 5648,
        'views': 2905288,
        'filename': '01_jubilee_mensrights_vs_feminists.en.vtt',
        'position': 'Center-left/Moderate',
        'format': 'Debate format - Multiple speakers from MRM and Feminism'
    },
    {
        'id': '3WMuzhQXJoY',
        'title': 'MEETING THE ENEMY A feminist comes to terms with the Men\'s Rights movement | Cassie Jaye | TEDxMarin',
        'channel': 'TEDx Talks',
        'duration': 887,
        'views': 13449094,
        'filename': '02_tedx_meeting_the_enemy.en.vtt',
        'position': 'Empathetic to MRM, Critical of mainstream feminism',
        'format': 'Solo talk with interview segments'
    },
    {
        'id': 'W6aDNutZoB0',
        'title': '"toxic" femininity: what\'s up with girlbloggers, female manipulators, and femcels?',
        'channel': 'Mina Le',
        'duration': 1739,
        'views': 3181702,
        'filename': '03_mina_le_toxic_femininity.en.vtt',
        'position': 'Analytical/Critical of feminine toxicity',
        'format': 'Commentary/Analysis video'
    },
    {
        'id': 'FJNnSlTZE9E',
        'title': 'The failure of feminist Hollywood',
        'channel': 'Sydney Watson',
        'duration': 820,
        'views': 710485,
        'filename': '04_sydney_watson_hollywood_failure.en.vtt',
        'position': 'Anti-feminist/Critical of feminist Hollywood',
        'format': 'Commentary video'
    },
    {
        'id': 'gX5Vaqx_nNg',
        'title': 'Jordan Peterson on "Toxic Femininity"',
        'channel': 'Jordan Peterson',
        'duration': 505,
        'views': 516077,
        'filename': '05_jordan_peterson_toxic_femininity.en.vtt',
        'position': 'Clinical psychologist, Anti-feminist',
        'format': 'Solo lecture/interview'
    }
]

transcript_dir = Path('/home/alca/youtube_deep/transcripts/')
output_dir = Path('/home/alca/youtube_deep/transcripts/')

for video in videos:
    vtt_path = transcript_dir / video['filename']
    if vtt_path.exists():
        print(f"Processing: {video['title']}")
        transcript = parse_vtt(vtt_path)
        extracted = extract_speakers_and_arguments(transcript, video['title'])
        
        output = {
            'video_id': video['id'],
            'title': video['title'],
            'channel': video['channel'],
            'duration_seconds': video['duration'],
            'view_count': video['views'],
            'speaker_position': video['position'],
            'format_type': video['format'],
            'transcript_text': transcript,
            'analysis': {
                'main_themes': extracted['main_themes'],
                'key_claims': extracted['key_claims'],
                'speakers_identified': extracted['speakers_identified']
            }
        }
        
        output_file = output_dir / f"{video['id']}_analysis.json"
        with open(output_file, 'w', encoding='utf-8') as f:
            json.dump(output, f, indent=2, ensure_ascii=False)
        print(f"  -> Saved to {output_file.name}")
    else:
        print(f"  -> File not found: {vtt_path}")

print("\nDone!")
