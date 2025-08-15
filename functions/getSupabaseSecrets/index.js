// EdgeOne serverless function
export default async function handler(req, res) {
  // EdgeOne environment variables
  const url = process.env.SUPABASE_URL;
  const anonKey = process.env.SUPABASE_ANON_KEY;

  if (!url || !anonKey) {
    return new Response(JSON.stringify({ error: 'Supabase keys not set' }), {
      status: 500,
      headers: { 'Content-Type': 'application/json' },
    });
  }

  return new Response(
    JSON.stringify({ url, anonKey }),
    { headers: { 'Content-Type': 'application/json' } }
  );
}

// export default function handler(req, res) {
//   // Read from environment variables defined in EdgeOne console
//   const supabaseUrl = process.env.SUPABASE_URL;
//   const supabaseKey = process.env.SUPABASE_ANON_KEY;

//   // Return as JSON
//   res.json({
//     supabaseUrl,
//     supabaseKey
//   });
// }