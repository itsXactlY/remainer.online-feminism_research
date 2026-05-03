import json, sys, time, urllib.request, urllib.parse

def fetch_json(url, retries=4):
    headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36'
    }
    for attempt in range(retries):
        try:
            req = urllib.request.Request(url, headers=headers)
            with urllib.request.urlopen(req, timeout=20) as resp:
                return json.loads(resp.read().decode())
        except Exception as e:
            print(f"  Attempt {attempt+1} failed: {e}", file=sys.stderr)
            if attempt < retries-1:
                time.sleep(5 * (attempt + 1))
    return None

def get_comments(post_id, subreddit, limit=25):
    url = f"https://www.reddit.com/r/{subreddit}/comments/{post_id}/.json?limit={limit}"
    data = fetch_json(url)
    if not data or len(data) < 2:
        return [], {}
    post_data = data[0]['data']['children'][0]['data']
    post_info = {
        'title': post_data.get('title', ''),
        'score': post_data.get('score', 0),
        'selftext': post_data.get('selftext', ''),
        'author': post_data.get('author', ''),
    }
    comments = []
    for c in data[1]['data']['children']:
        cd = c['data']
        if cd.get('body'):
            comments.append({
                'id': cd.get('id', ''),
                'author': cd.get('author', ''),
                'body': cd.get('body', ''),
                'score': cd.get('score', 0),
            })
    return comments, post_info

# Load existing data
with open('/home/alca/reddit_deep/threads_full.json', 'r') as f:
    existing = json.load(f)

# Try to find missing posts - with longer delay
queries = [
    ('GenderCynical', 'most active feminist community'),
    ('AskFeminists', 'feminists you do not consider allies'),
    ('MensRights', 'yes feminism is misandry'),
]

for subreddit, query in queries:
    print(f"\n=== Searching r/{subreddit} for: {query} ===")
    time.sleep(3)
    url = f"https://www.reddit.com/r/{subreddit}/search.json?q={urllib.parse.quote(query)}&sort=top&restrict_sr=1&limit=10"
    data = fetch_json(url)
    if data and 'data' in data and data['data']['children']:
        for p in data['data']['children'][:2]:
            pd = p['data']
            print(f"  Found: {pd['title'][:70]} (id: {pd['id']}, score: {pd['score']})")
            comments, post_info = get_comments(pd['id'], subreddit, 25)
            if comments:
                if subreddit not in existing:
                    existing[subreddit] = {'posts': []}
                existing[subreddit]['posts'].append({
                    'id': pd['id'],
                    'title': pd['title'],
                    'score': pd['score'],
                    'selftext': pd.get('selftext', ''),
                    'comments': comments
                })
                print(f"    Added {len(comments)} comments")
            time.sleep(2)
    else:
        print(f"  No results found")

# Save updated
with open('/home/alca/reddit_deep/threads_full.json', 'w') as f:
    json.dump(existing, f, indent=2)

total_posts = sum(len(d['posts']) for d in existing.values())
total_comments = sum(sum(len(p['comments']) for p in d['posts']) for d in existing.values())
print(f"\n=== FINAL: {total_posts} posts, {total_comments} comments ===")
