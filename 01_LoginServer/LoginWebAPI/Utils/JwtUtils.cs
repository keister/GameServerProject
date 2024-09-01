using System.IdentityModel.Tokens.Jwt;
using LoginWebAPI.Authentication;
using Microsoft.Extensions.Options;
using Microsoft.IdentityModel.Tokens;
using System.Security.Claims;
using System.Text;
using JwtRegisteredClaimNames = Microsoft.IdentityModel.JsonWebTokens.JwtRegisteredClaimNames;
using System.Security.Principal;
using Microsoft.IdentityModel.JsonWebTokens;

namespace LoginWebAPI.Utils
{

    public class ValidationResult
    {
        public UInt64 Id { get; set; }
        public string UserName { get; set; }
    }

    public class JwtUtils
    {
        JwtSettings _jwtSettings;
        private TokenValidationParameters _validationParameters;
        public JwtUtils(IOptions<JwtSettings> option)
        {
            _jwtSettings = option.Value;
            _validationParameters = new TokenValidationParameters()
            {
                ValidIssuer = _jwtSettings.Issuer,
                ValidAudience = _jwtSettings.Audience,
                IssuerSigningKey = new SymmetricSecurityKey(Encoding.UTF8.GetBytes(_jwtSettings.Key)),
                ClockSkew = TimeSpan.Zero,
                ValidateIssuer = true,
                ValidateAudience = true,
                ValidateIssuerSigningKey = true,
            };
        }

        public string GenerateToken(Account account)
        {
            var key = new SymmetricSecurityKey(Encoding.UTF8.GetBytes(_jwtSettings.Key));

            var credentials = new SigningCredentials(key, SecurityAlgorithms.HmacSha256Signature);
            var claims = new[]
            {
                new Claim(JwtRegisteredClaimNames.Sub, account.Id.ToString()),
                new Claim("username", account.UserName),
                new Claim(JwtRegisteredClaimNames.Jti, Guid.NewGuid().ToString()),
            };

            var token = new JwtSecurityToken(
                issuer: _jwtSettings.Issuer,
                audience: _jwtSettings.Audience,
                claims: claims,
                expires: DateTime.Now.AddHours(99999),
                signingCredentials: credentials
            );

            return new JwtSecurityTokenHandler().WriteToken(token);
        }

        public ValidationResult? ValidateToken(string token)
        {
            try
            {
                var tokenHandler = new JwtSecurityTokenHandler();

                var principal = tokenHandler.ValidateToken(token, _validationParameters, out SecurityToken validToken);
                JwtSecurityToken tt = (JwtSecurityToken)validToken;
                var result = new ValidationResult()
                {
                    UserName = tt.Claims.FirstOrDefault(c => c.Type == "username")!.Value,
                    Id = UInt64.Parse(tt.Claims.FirstOrDefault(c => c.Type == JwtRegisteredClaimNames.Sub)!.Value)
                };

                return result;

            }
            catch (SecurityTokenValidationException e)
            {
                return null;
            }
        }

    }
}
