// EdgeOne serverless function
export default function handler(req, res) {
  // Read from environment variables defined in EdgeOne console
  const supabaseUrl = process.env.SUPABASE_URL;
  const supabaseKey = process.env.SUPABASE_ANON_KEY;

  // Return as JSON
  res.json({
    supabaseUrl,
    supabaseKey
  });
}