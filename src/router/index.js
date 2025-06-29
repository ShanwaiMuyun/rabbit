import { createRouter, createWebHistory } from 'vue-router'
import Login from '@/views/Login/index.vue'
import Layout from '@/views/Layout/index.vue'
import Home from '@/views/Home/index.vue'
import Category from '@/views/Category/index.vue'
import SubCategory from '@/views/SubCategory/index.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    // 一级路由
    {
      path: '/',
      component: Layout,
      // 二级路由
      children: [
        {
          path: '',// 小技巧：当path为''时，表示该路由为默认路由，即访问http://localhost:8080/时，会自动跳转到http://localhost:8080/#/home
          component: Home,
        },
        {
          path: 'category/:id',
          component: Category,
        },
        {
          path: 'category/sub/:id',
          component: SubCategory,
        }
      ]
    },
    {
      path: '/login',
      component: Login,
    }
  ],
})

export default router
