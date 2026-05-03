import json
import urllib.request
import time
import os

SUBREDDITS = [
    "TrollCoping",
    "PoliticalCompassMemes",
    "FeMRADebates",
    "AskFeminists",
    "MensRights",
    "PinkFlamenco",
    "GenderCynical",
    "LeftWingMaleAdvocates"
]

SEARCH_TERMS = [
    "toxic feminism",
    "feminist extremism",
    "man hating feminism",
    "misandry feminism",
    "gender ideology",
    "radfem vs liberal",
    "TERF drama"
]

def fetch_subreddit_search(subreddit, term, limit=25):
    """Fetch search results from a subreddit using Reddit's JSON API."""
    query = urllib.parse.quote(term)
    url = f"https://www.reddit.com/r/{subreddit}/search.json?q={query}&restrict_sr=1&sort=relevance&limit={limit}"
    
    headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36'
    }
    
    req = urllib.request.Request(url, headers=headers)
    
    try:
        with urllib.request.urlopen(req, timeout=10) as response:
            data = json.loads(response.read().decode())
            return data
    except Exception as e:
        print(f"Error fetching r/{subreddit} for '{term}': {e}")
        return None

def parse_posts(data):
    """Parse Reddit JSON response to extract post details."""
    posts = []
    if not data or 'data' not in data:
        return posts
    
    children = data['data'].get('children', [])
    for child in children:
        post_data = child['data']
        posts.append({
            'title': post_data.get('title', ''),
            'score': post_data.get('score', 0),
            'num_comments': post_data.get('num_comments', 0),
            'url': post_data.get('url', ''),
            ' permalink': post_data.get('permalink', ''),
            'created_utc': post_data.get('created_utc', 0),
            'selftext': post_data.get('selftext', '')[:500] if post_data.get('selftext') else ''
        })
    return posts

import urllib.parse

os.makedirs('/home/alca/reddit_gender_wars/results', exist_ok=True)

all_results = {}

for subreddit in SUBREDDITS:
    print(f"\n=== Processing r/{subreddit} ===")
    subreddit_results = {}
    
    for term in SEARCH_TERMS:
        print(f"  Searching '{term}'...")
        data = fetch_subreddit_search(subreddit, term, limit=20)
        
        if data:
            posts = parse_posts(data)
            subreddit_results[term] = posts
            print(f"    Found {len(posts)} posts")
        else:
            subreddit_results[term] = []
        
        time.sleep(1)  # Rate limiting
    
    all_results[subreddit] = subreddit_results

# Save combined results
with open('/home/alca/reddit_gender_wars/results/combined_results.json', 'w') as f:
    json.dump(all_results, f, indent=2)

# Generate summary report
report = []
report.append("=" * 80)
report.append("REDDIT GENDER WARS - TOXIC FEMINISM THREADS SUMMARY")
report.append("=" * 80)
report.append(f"\nSubreddits analyzed: {', '.join(SUBREDDITS)}")
report.append(f"Search terms: {', '.join(SEARCH_TERMS)}")
report.append("\n" + "-" * 80)

total_posts = 0
for subreddit, terms in all_results.items():
    report.append(f"\n\n### r/{subreddit} ###")
    term_count = 0
    for term, posts in terms.items():
        if posts:
            term_count += len(posts)
            total_posts += len(posts)
            report.append(f"\n  '{term}' ({len(posts)} posts):")
            for p in posts[:5]:  # Top 5 per term
                report.append(f"    - [{p['score']} pts] {p['title'][:70]}...")
                report.append(f"      Comments: {p['num_comments']}")
    if term_count == 0:
        report.append("  (No posts found)")

report.append(f"\n\n{'=' * 80}")
report.append(f"TOTAL POSTS COLLECTED: {total_posts}")
report.append("=" * 80)

report_text = "\n".join(report)
with open('/home/alca/reddit_gender_wars/results/summary_report.txt', 'w') as f:
    f.write(report_text)

print(f"\nDone! Total posts: {total_posts}")
print(f"Results saved to /home/alca/reddit_gender_wars/results/")
