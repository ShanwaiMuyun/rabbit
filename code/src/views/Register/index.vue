<script setup>
import { ref } from 'vue'
import { ElMessage } from 'element-plus'
import { useRouter } from 'vue-router'
import { useUserStore } from '@/stores/userStore'

const router = useRouter()
const userStore = useUserStore()
const formRef = ref(null)

const form = ref({
    account: '',
    password: '',
    confirmPassword: '',
    role: 'buyer'
})

const rules = {
    account: [
        { required: true, message: '请输入用户名', trigger: 'blur' }
    ],
    password: [
        { required: true, message: '请输入密码', trigger: 'blur' },
        { min: 6, max: 14, message: '密码长度为 6-14 个字符', trigger: 'blur' }
    ],
    confirmPassword: [
        { required: true, message: '请再次输入密码', trigger: 'blur' },
        {
            validator: (_, value, callback) => {
                if (value !== form.value.password) {
                    callback(new Error('两次输入的密码不一致'))
                } else {
                    callback()
                }
            },
            trigger: 'blur'
        }
    ],
    role: [
        { required: true, message: '请选择注册身份', trigger: 'change' }
    ]
}

const doRegister = () => {
    formRef.value.validate(async (valid) => {
        if (!valid) {
            return
        }

        await userStore.register({
            account: form.value.account,
            password: form.value.password,
            role: form.value.role
        })

        ElMessage.success('注册成功，请等待管理员审核')
        router.push('/login')
    })
}
</script>

<template>
    <div>
        <header class="register-header">
            <div class="container">
                <h1 class="logo">
                    <RouterLink to="/">商城</RouterLink>
                </h1>
                <RouterLink class="entry" to="/login">已有账号，去登录</RouterLink>
            </div>
        </header>

        <section class="register-section">
            <div class="wrapper">
                <nav>用户注册</nav>
                <el-form ref="formRef" :model="form" :rules="rules" label-width="76px" status-icon>
                    <el-form-item prop="account" label="用户名">
                        <el-input v-model="form.account" placeholder="请输入用户名" />
                    </el-form-item>
                    <el-form-item prop="password" label="密码">
                        <el-input v-model="form.password" type="password" show-password placeholder="请输入密码" />
                    </el-form-item>
                    <el-form-item prop="confirmPassword" label="确认密码">
                        <el-input v-model="form.confirmPassword" type="password" show-password placeholder="请再次输入密码" />
                    </el-form-item>
                    <el-form-item prop="role" label="身份">
                        <el-radio-group v-model="form.role">
                            <el-radio-button label="buyer">买家</el-radio-button>
                            <el-radio-button label="seller">卖家</el-radio-button>
                        </el-radio-group>
                    </el-form-item>
                    <el-button size="large" class="subBtn" @click="doRegister">提交注册</el-button>
                </el-form>
            </div>
        </section>
    </div>
</template>

<style scoped lang="scss">
.register-header {
    background: #fff;
    border-bottom: 1px solid #e4e4e4;

    .container {
        height: 110px;
        display: flex;
        align-items: center;
        justify-content: space-between;
    }

    .logo {
        width: 200px;

        a {
            display: block;
            height: 86px;
            width: 100%;
            text-indent: -9999px;
            background: url("@/assets/images/logo.png") no-repeat center / contain;
        }
    }

    .entry {
        font-size: 16px;
        color: $xtxColor;
    }
}

.register-section {
    background: url('@/assets/images/login-bg.png') no-repeat center / cover;
    min-height: 520px;
    position: relative;

    .wrapper {
        width: 420px;
        background: #fff;
        position: absolute;
        left: 50%;
        top: 54px;
        transform: translate3d(80px, 0, 0);
        box-shadow: 0 0 10px rgba(0, 0, 0, 0.15);
        padding: 0 24px 28px;

        nav {
            height: 64px;
            line-height: 64px;
            margin-bottom: 22px;
            border-bottom: 1px solid #f5f5f5;
            text-align: center;
            font-size: 20px;
        }
    }
}

.subBtn {
    background: $xtxColor;
    width: 100%;
    color: #fff;
}
</style>
