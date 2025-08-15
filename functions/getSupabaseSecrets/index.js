// EdgeOne serverless function
export async function onRequest({ request, params, env }) {

  try {
    // EdgeOne environment variables
    const url = env.SUPABASE_URL;
    const anonKey = env.SUPABASE_ANON_KEY;

    if (!url || !anonKey) {
      return new Response(JSON.stringify({ error: 'Supabase keys not set', url: env.SUPABASE_URL, anonKey: env.SUPABASE_ANON_KEY}), {
        status: 500,
        headers: {
            'content-type': 'application/json; charset=UTF-8',
            'Access-Control-Allow-Origin': '*',
        }, 
      });
    }

    const res = JSON.stringify({ url, anonKey });

    return new Response(res, { 
      headers: {
          'content-type': 'application/json; charset=UTF-8',
          'Access-Control-Allow-Origin': '*',
      }, 
    });
  } catch (err) {
    console.log(err);
    return new Response(
      JSON.stringify({
        error: "Failed to get Supabase url and key.",
        message: err.message,
        stack: err.stack,
        name: err.name
      }),
      {
        headers: {
          'content-type': 'application/json; charset=UTF-8',
          'Access-Control-Allow-Origin': '*',
        },
      }
    );
  }

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