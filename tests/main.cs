
function add(left:double, right:double) {
    return left + right
}

function fibonacci(n: double) {
    if n <= 0 {
        return 0
    }
    if n == 1 {
        return 1
    }
    return fibonacci(n - 1) + fibonacci(n - 2)
}

print(add(1, 2))
print(fibonacci(10))